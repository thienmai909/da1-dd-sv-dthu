#include <csv.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <stdexcept>

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