#include <csv.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <concepts>
#include <optional>
#include <limits>

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
    template<std::integral T>
    struct Range {
        T _max = std::numeric_limits<T>::max();
        T _min = std::numeric_limits<T>::min();

        constexpr Range (T min, T max) : _min(min), _max(max) {}
        constexpr Range () {}
    };

    struct Options {
        Option allowEmpty = Option::None;  //cho trả về trống
        Option allowCancel = Option::None; //cho hủy nhập
        int maxRetry = -1;        //số lần lặp lại, -1 là không lặp

        constexpr Options(int retry) : maxRetry(retry) {}
        constexpr Options() {}
    };

    std::optional<int> readInt(
        const std::string& prompt = "",
        std::optional<Range<int>> range = std::nullopt,
        std::optional<Options> options = std::nullopt
    ) {
        Options opt;
        if (options.has_value()) opt = *options;

        int retry = 0;
        while (true) {
            if (!prompt.empty()) {
                std::cout << prompt;
                if (opt.allowCancel == Option::Ok) std::cout << " (q = cancel)";
                std::cout << ": ";
            }

            std::string input;
            std::getline(std::cin, input);
            
            if (opt.allowCancel == Option::Ok && input == "q")
                return std::nullopt;

            if (input.empty()) {
                if (opt.allowEmpty == Option::Ok)
                    return std::nullopt;
                std::cout << "Not empty!\n";
                goto retry_check;    
            }
            
            try {
                std::size_t pos;
                int value = std::stoi(input, &pos);

                if (pos != input.size())
                    throw std::invalid_argument("Invalid.\n");
                
                if (range.has_value())
                    if (value < range->_min || value > range->_max) {
                        std::cout << "Range[" << range->_min << ", " << range->_max << "]\n";
                        goto retry_check;
                    }
                return value;

            } catch(...) {
                std::cout << "Invalid.\n";
            }

            retry_check:
            if (opt.maxRetry != -1 && ++retry >= opt.maxRetry)
                return std::nullopt;
        }
    }

    inline std::optional<int> readInt(const std::string& prompt) {
        
        return readInt(prompt, std::nullopt, Options());
    }

    inline std::optional<int> readIntRange(
        const std::string& prompt,
        int min, int max
    ) {
        return readInt(prompt, Range<int>(min, max), std::nullopt);
    }

    inline std::optional<int> readIntRangeRetry(
        const std::string& prompt,
        int min, int max, int retry
    ) {
        return readInt(prompt, Range<int>(min, max), Options(retry));
    }

    inline std::optional<int> readIntCancelable(
        const std::string& prompt,
        int min, int max
    ) {
        Options opt;
        opt.allowCancel = Option::Ok;
        return readInt(prompt, Range<int>(min, max), opt);
    }
}