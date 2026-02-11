#include "query.h"
#include "../context/db_context.h"
#include "../parser/parser.h"
#include "../core/metadata.h"
#include "../heap_table/heap_table.h"
#include "../page/pager.h"
#include "../core/tuple_encoder.h"
#include <iostream>

void execute_update(const std::string sql){

    if (db_ctx.current_database.empty()) {
        std::cerr << "No database selected\n";
        return;
    }

    UpdateQuery uq;

    try{
        uq = parse_update_query(sql);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return;
    }

    Table_info ti;

    try {
        ti = load_table_metadata(db_ctx.current_database, uq.table_name);
    }
    catch (...) {
        std::cerr << "Failed to load table metadata\n";
        return;
    }

    std::string db_file =
    "../data/" + db_ctx.current_database + "/" +
    uq.table_name + "/" + uq.table_name + ".db";

    Pager pager(db_file);
    HeapTable table(pager);

    int updated = 0;

    auto rows = table.scan_with_rid();

    for(auto& row : rows){
        auto value = decode_row(row.data); //values = ["1", "Alice", "Math"]
 
        if(uq.where.exists){
           bool matched = false;

            for (size_t i = 0; i < ti.attributes.size(); i++) {
                if (ti.attributes[i].first == uq.where.column) {
                    if (i < value.size()) {
                        matched = eval_condn(
                            value[i],
                            uq.where.op,
                            uq.where.value
                        );
                    }
                    break;
                }
            }

            if(!matched){
                continue;
            }

        }

        for(const auto& [col , new_val] : uq.new_val){
            for(size_t i=0;i<ti.attributes.size();i++){
                if(ti.attributes[i].first == col){
                    if(i < value.size()){
                        value[i] = new_val;
                    }
                }
            }
        }

        auto new_tuple = encode_row(value);

        if(table.delete_tuple(row.rid)){
            table.insert(new_tuple.data(),new_tuple.size());
            updated++;
        }



    }  
    
    std::cout << updated << " row(s) updated\n";


}