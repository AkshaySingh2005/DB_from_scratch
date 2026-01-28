#include "row_reader.h"
#include <filesystem>
#include <fstream>
#include <cstring>

namespace fs = std::filesystem ;

std::vector<Row> read_all_rows( const std::string& db, const Table_info& table ) {

    std::vector<Row> rows;

    fs::path bin_path = fs::path("../data")/db/table.table_name/(table.table_name + ".bin");

    std::ifstream in(bin_path,std::ios::binary);

    if(!in){
        return rows;
    }


    const size_t COL_SIZE = 256;
    const size_t ROW_SIZE = table.attributes.size() * COL_SIZE;

    std::vector<char>buffer(ROW_SIZE);

    while(in.read(buffer.data(),  ROW_SIZE)){
        Row row;

        for (size_t i = 0; i < table.attributes.size(); i++) {
            char* start = buffer.data() + i * COL_SIZE;
            row.values.emplace_back(start);
        }

        rows.push_back(row);
    }

    return rows;
}
