#include "query.h"
#include "../core/db_context.h"
#include "../parser/parser.h"
#include "../core/metadata.h"
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

    Table_info ti;

    try{
        ti = load_table_metadata(db_ctx.current_database,sq.table_name);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return;
    }
    
    std::unordered_set<std::string>valid_cols;

    for(const auto& [name,type] : ti.attributes){
        valid_cols.insert(name);
    }


    if(sq.columns.size() == 1 && sq.columns[0] == "*"){
        std::cout<<"Selecting all columns"<<std::endl;
        // show data from bin all columns
        return;
    }

    for(const auto& col : sq.columns){
        if(valid_cols.find(col) == valid_cols.end()){
            std::cerr << "Unknown column '" << col << "'\n";
            return;
        }
    }

    std::cout << "Columns verified successfully\n";

}
