#include "csv.hpp"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <concepts>
#include <optional>
#include <limits>
#include <charconv>

enum class Command {
    Add, Update, Delete, List, Exit
};

enum class Option {
    Ok, None
};

namespace utility_csv {

    using Cell = std::string;
    using Row = std::vector<Cell>;
    using Rows = std::vector<Row>;

    struct CSVData {
        Row headers;
        Rows rows;

        bool empty() const {
            return rows.empty();
        }

        std::size_t row_count() const {
            return rows.size();
        }

        std::size_t column_count() const {
            return headers.size();
        }
    };
    
    CSVData read_csv(
        const std::filesystem::path& file,
        char delimiter = ',',
        bool trim = true
    ) {
        if (!std::filesystem::exists(file))
            throw std::runtime_error("CSV file not found: " + file.string());
        
        if (!std::filesystem::is_regular_file(file))
            throw std::runtime_error("Not a regular file: " + file.string());
        
        if (std::filesystem::file_size(file) == 0)
            throw std::runtime_error("CSV file is empty: " + file.string());
        
        csv::CSVFormat format;
        format.delimiter(delimiter);
        format.variable_columns(true); // true sẽ cho throw
                                       // false sẽ in ra cho dù có thiếu cell
                                       // không có sẽ bỏ qua row đó luôn
        
        if (trim)
            format.trim({ ' ', '\t' });
        
        csv::CSVReader reader(file.string(), format);
        
        CSVData data;
        
        data.headers = reader.get_col_names();
        if (data.headers.empty())
            throw std::runtime_error("CSV has no header: " + file.string());
        
        const std::size_t column_count = data.headers.size();
        std::size_t row_index = 1;

        for (auto& row : reader) {
            ++row_index;

            if (row.size() != column_count)
                throw std::runtime_error("CSV integrity error at line " + std::to_string(row_index)
                                        + ": expected " + std::to_string(column_count) 
                                        + " columns, got " + std::to_string(row.size())
                );

            Row r;
            r.reserve(column_count);
            
            for (auto& cell : row) 
                r.push_back(cell.get<std::string>());
            
            data.rows.push_back(std::move(r));
        }

        return data;
    }

    void write_csv(
        const std::filesystem::path& filename,
        const CSVData& data
    ) {
        if (data.empty()) 
            throw std::runtime_error("CSV headers are empty");
        
        const std::size_t column_count = data.headers.size();
        if (column_count == 0) 
            throw std::runtime_error("CSV has no headers");

        for (const auto& r : data.rows)
            if (r.size() > column_count)
                throw std::runtime_error("Row has more columns than header");
        
        std::ofstream ofs(filename.string(), std::ios::binary);
        if (!ofs) 
            throw std::runtime_error("Failed to open file: " + filename.string());

        auto writer = csv::make_csv_writer(ofs);
        writer << data.headers;
        for (const auto& row : data.rows) 
            writer << row;
    }
}

namespace utility_input {
    template <typename T>
    concept int_or_float = std::is_integral_v<T> || std::is_floating_point_v<T>;

    template <typename T>
    concept int_or_float_or_string = std::is_integral_v<T> || 
                                     std::is_floating_point_v<T> ||
                                     std::convertible_to<T, std::string>;

    template <int_or_float T>
    struct Range {
        T _max = std::numeric_limits<T>::max();
        T _min = std::numeric_limits<T>::min();
        bool _enabled = true;

        constexpr Range (T min, T max, bool enabled) : _min(min), _max(max), _enabled(enabled) {}
        constexpr Range (T min, T max) : _min(min), _max(max) {}
        constexpr Range (bool enabled) : _enabled(enabled) {}
        constexpr Range () {}
    };

    struct Options {
        Option allowEmpty = Option::None;  //cho trả về trống
        Option allowCancel = Option::None; //cho hủy nhập
        int maxRetry = -1;        //số lần lặp lại, -1 là không lặp

        constexpr Options(Option empty, Option cancel, int retry) : allowEmpty(empty), allowCancel(cancel), maxRetry(retry) {}
        constexpr Options(Option empty, Option cancel) : allowEmpty(empty), allowCancel(cancel) {}
        constexpr Options(int retry) : maxRetry(retry) {}
        constexpr Options() {}
    };

    struct Prompt {
        std::string text;

        bool showCancelHint = true;
        bool showRetryHint  = true;
        bool showRangeHint  = true;

        constexpr Prompt(const std::string& t) : text(t) {}
        constexpr Prompt() = default;
    };

    template <int_or_float T>
    void showRangeHint(const Range<T>& range) {
        if (!range._enabled) return;
        std::cout << "  [Range: " << range._min << ".." << range._max << "]";
    }

    template <typename T>
    bool parse(const std::string&, T&) {
        static_assert(sizeof(T) == 0, "parse<T>: unsupported type");
        return false;
    }

    template <int_or_float T>
    bool parse(const std::string& input, T& out) {
        T value{};
        auto [ptr, ec] = std::from_chars(
            input.data(), input.data() + input.size(), value
        );

        if (ec != std::errc{}) return false;
        if (ptr != input.data() + input.size()) return false;

        out = value;
        return true;
    }

    template <>
    bool parse<std::string>(const std::string& input, std::string& out) {
        if (input.empty()) return false;
        out = input;
        return true;
    }

    template <int_or_float_or_string T>
    bool validate(const T&) {
        return true;
    }

    template <int_or_float T>
    bool validate(const T& value, const Range<T>& range) {
        if (!range._enabled) return true;
        return value >= range._min && value <= range._max;
    }

    template <int_or_float In, int_or_float_or_string Out>
    std::optional<Out> readCore(
        const Prompt& prompt,
        const Range<In>& range,
        const Options& opt
    ) {
        std::string input;
        int retry = 1;

        while (opt.maxRetry < 0 || retry <= opt.maxRetry) {
            if (!prompt.text.empty()){
                if (prompt.showRetryHint || prompt.showRangeHint || prompt.showCancelHint)
                    std::cout << "Note:";

                if constexpr (int_or_float<Out>)
                    if (prompt.showRangeHint)
                        showRangeHint(range);

                if (prompt.showCancelHint && opt.allowCancel == Option::Ok)
                    std::cout << "  (type 'cancel' to cancel)";
                
                if (prompt.showRetryHint && opt.maxRetry > 0) {
                    std::cout << "  [retry " << retry << "/" << opt.maxRetry << "]";
                }
                if ((prompt.showRetryHint || prompt.showRangeHint || prompt.showCancelHint))
                    std::cout << std::endl;
            }

            std::cout << prompt.text;

            if (!std::getline(std::cin, input)) 
                return std::nullopt;

            if (opt.allowCancel == Option::Ok && input == "cancel") {
                std::cout << "Enter cancelled.\n";
                return std::nullopt;
            }

            if (input.empty()) {
                if (opt.allowEmpty == Option::Ok) {
                    std::cout << "Accept empty.\n";
                    return std::nullopt;
                }
                std::cout << "Not empty.\n";
                ++retry;
                continue;
            }

            Out value{};
            if (!parse<Out>(input, value)) {
                std::cout << "Wrong format.\n";
                ++retry;
                continue;
            }

            if constexpr (int_or_float<Out>)
                if (!validate(value, range)) {
                    std::cout << "Out range.\n";
                    ++retry;
                    continue;
                }

            return value;
        }
        std::cout << "Exceeded the allowed number of times.\n";
        return std::nullopt;
    }

    template <std::integral T>
    inline std::optional<T> readInt(
        const std::string& prompt,
        T min = std::numeric_limits<T>::min(),
        T max = std::numeric_limits<T>::max(),
        const Options& opt = {}
    ) {
        Prompt p{ prompt };
        Range<T> r{ min, max, true };
        return readCore<T, T>(p, r, opt);
    }

    template <std::floating_point T>
    inline std::optional<T> readFloat(
        const std::string& prompt,
        T min = std::numeric_limits<T>::lowest(),
        T max = std::numeric_limits<T>::max(),
        const Options& opt = {}
    ) {
        Prompt p{ prompt };
        Range<T> r{ min, max, true };
        return readCore<T, T>(p, r, opt);
    }

    inline std::optional<std::string> readString(
        const std::string& prompt,
        bool empty = false
    ) {
        Prompt p{ prompt };
        p.showRetryHint = false;
        p.showRangeHint = false;
        p.showCancelHint = false;

        Range<int> dummyRange{ false };

        Options opt{};
        opt.allowEmpty = empty ? Option::Ok : Option::None;
        opt.allowCancel = Option::None;

        return readCore<int, std::string>(p, dummyRange, opt);
    }
}

