#include "query.h"
#include "../core/db_context.h"
#include "../parser/parser.h"
#include "../core/metadata.h"
#include "../core/row_reader.h"
#include <iostream>
#include <unordered_set>


void execute_select(const std::string& sql) {

    if(db_ctx.current_database.empty()){
        std::cerr << "No database selected\n";
        return;
    }

    SelectQuery sq;

    try{
        sq = parse_select_query(sql);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return;
    }

    // load metadata
    Table_info ti;
    try{
        ti = load_table_metadata(db_ctx.current_database,sq.table_name);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return;
    }

    std::vector<size_t> col_indexes;
    
    // SELECT *

    if (sq.columns.size() == 1 && sq.columns[0] == "*") {
        for (size_t i = 0; i < ti.attributes.size(); i++) {
            col_indexes.push_back(i);
        }
    }
    // SELECT col1, col2

    else {
        for (const auto& col : sq.columns) {
            bool found = false;

            for (size_t i = 0; i < ti.attributes.size(); i++) {
                if (ti.attributes[i].first == col) {
                    col_indexes.push_back(i);
                    found = true;
                    break;   
                }
            }

            if (!found) {
                std::cerr << "Unknown column '" << col << "'\n";
                return;
            }
        }
    }

    
    auto rows = read_all_rows(db_ctx.current_database, ti);

  
    for (const auto& row : rows) {
        for (size_t idx : col_indexes) {
            std::cout << row.values[idx] << " ";
        }
        std::cout << '\n';
    }
}