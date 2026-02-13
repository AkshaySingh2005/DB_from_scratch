#include "query.h"
#include "../context/db_context.h"
#include "../parser/parser.h"
#include "../core/metadata.h"
#include "../heap_table/heap_table.h"
#include "../page/pager.h"
#include "../core/tuple_encoder.h"
#include "../core/storage_manager.h"
#include <iostream>

void execute_delete(const std::string sql){

    if (db_ctx.current_database.empty()) {
        std::cerr << "No database selected\n";
        return;
    }

    DeleteQuery dq;

    try{
        dq = parse_delete_query(sql);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return;
    }

    Table_info ti;

    try {
        ti = load_table_metadata(db_ctx.current_database, dq.table_name);
    }
    catch (...) {
        std::cerr << "Failed to load table metadata\n";
        return;
    }

    open_table_storage(dq.table_name);
    HeapTable table(*db_ctx.buffer);


    auto rows = table.scan_with_rid();
    
    int del = 0;

    for(auto& row : rows){
        auto value = decode_row(row.data);

        if(dq.where.exists){
           bool matched = false;

            for (size_t i = 0; i < ti.attributes.size(); i++) {
                if (ti.attributes[i].first == dq.where.column) {
                    if (i < value.size()) {
                        matched = eval_condn(
                            value[i],
                            dq.where.op,
                            dq.where.value
                        );
                    }
                    break;
                }
            }

            if(!matched){
                continue;
            }

        }

        if(table.delete_tuple(row.rid)){
            del++;
        }

    }

    db_ctx.buffer->flush_all();
    
    std::cout << del << " row(s) deleted\n";

}

