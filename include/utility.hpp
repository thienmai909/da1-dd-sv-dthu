#include <csv.hpp>
#include <string>
#include <vector>
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
        
        
        csv::CSVFormat format;
        format.delimiter(delimiter);
        
        if (trim)
            format.trim({ ' ', '\t' });
        
        csv::CSVReader reader(file.string(), format);
        
        CSVData data;
        
        for (auto& h : reader.get_col_names())
            data.headers.push_back(h);
                
        for (auto& row : reader) {
            Row r;
            r.reserve(row.size());
            
            for (auto& cell : row)
                r.push_back(cell.get<>());
            
            data.rows.push_back(std::move(r));
        }
        
        return data;
    }
}