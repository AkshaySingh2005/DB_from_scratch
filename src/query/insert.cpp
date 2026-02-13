#include "query.h"
#include "../context/db_context.h"
#include "../core/tuple_encoder.h"
#include "../parser/parser.h"
#include "../heap_table/heap_table.h"
#include "../core/metadata.h"
#include "../core/storage_manager.h"
#include <iostream>

void execute_insert(const std::string& sql) {

    
    auto args = parse_args(sql);

    if (args.size() < 3) {
        std::cerr << "Invalid INSERT syntax\n";
        return;
    }

    if (db_ctx.current_database.empty()) {
        std::cerr << "No database selected\n";
        return;
    }

    std::string table_name = args[2];

    
    Table_val tv = parse_insert_values_query(sql);

    
    Table_info ti;
    try {
        ti = load_table_metadata(db_ctx.current_database, table_name);
    } catch (...) {
        std::cerr << "Failed to load table metadata\n";
        return;
    }

    if (tv.values.size() != ti.attributes.size()) {
        std::cerr << "Column count mismatch\n";
        return;
    }

    
    auto tuple = encode_row(tv.values);


    open_table_storage(table_name);

    
    HeapTable table(*db_ctx.buffer);

    
    table.insert(tuple.data(), tuple.size());

    db_ctx.buffer->flush_all();

    std::cout << "1 row inserted\n";

}
