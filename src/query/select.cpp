#include "query.h"
#include "../context/db_context.h"
#include "../parser/parser.h"
#include "../core/metadata.h"
#include "../core/tuple_encoder.h"
#include "../page/pager.h"
#include "../heap_table/heap_table.h"
#include <iostream>
#include <unordered_set>

bool eval_condn(const std::string& lhs , const std::string& op , const std::string& rhs){
    if(op == "="){
        return lhs==rhs;
    }

    if(op == "!=" || op == "<>"){
        return lhs!=rhs;
    }

    double left = std::stod(lhs);
    double right = std::stod(rhs);

    if (op == "<")
        return left < right;

    if (op == ">")
        return left > right;

    if (op == "<=")
        return left <= right;

    if (op == ">=")
        return left >= right;

    return false;

}

void execute_select(const std::string& sql) {

    if (db_ctx.current_database.empty()) {
        std::cerr << "No database selected\n";
        return;
    }

    SelectQuery sq;

    try {
        sq = parse_select_query(sql);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return;
    }

    // Load metadata
    
    Table_info ti;
    try {
        ti = load_table_metadata(db_ctx.current_database, sq.table_name);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return;
    }

    // Determine column indexes
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

    // Open table storage using HeapTable
    std::string db_file =
        "../data/" + db_ctx.current_database +
        "/" + sq.table_name +
        "/" + sq.table_name + ".db";


    Pager pager(db_file);
    HeapTable heap_table(pager);

    auto tuples = heap_table.scan_all();

    // Print header
    for (size_t idx : col_indexes) {
        std::cout << ti.attributes[idx].first << " ";
    }
    std::cout << "\n";

    // Print rows
    for (const auto& tuple : tuples) {
        auto values = decode_row(tuple);

        if(sq.where.exits){
            bool matched = false;

            for(size_t i=0;i<ti.attributes.size();i++){
                if(ti.attributes[i].first == sq.where.column){
                    if(i < values.size()){
                        matched = eval_condn(
                            values[i],
                            sq.where.op,
                            sq.where.value
                        );
                    }
                    break;
                }
            }

            if(!matched){
                continue;
            }
        }

        for (size_t idx : col_indexes) {
           if (idx < values.size())
            std::cout << values[idx] << " ";
        }
        std::cout << "\n";   
    }
}
