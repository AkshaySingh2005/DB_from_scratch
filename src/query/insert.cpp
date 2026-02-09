#include "query.h"
#include "../context/db_context.h"
#include "../core/tuple_encoder.h"
#include "../parser/parser.h"
#include "../page/pager.h"
#include "../heap_table/heap_table.h"
#include "../core/metadata.h"
#include <iostream>



void execute_insert(const std::string& sql) {
    auto args = parse_args(sql);
    std::string table_name = args[2];

    Table_val tv = parse_insert_values_query(sql);
    Table_info ti = load_table_metadata(db_ctx.current_database, table_name);

    if (tv.values.size() != ti.attributes.size()) {
        std::cerr << "Column count mismatch\n";
        return;
    }

    auto tuple = encode_row(tv.values);

    std::string db_file = "../data/" + db_ctx.current_database + "/" + table_name + "/" + table_name + ".db";

    Pager pager(db_file);
    HeapTable table(pager);

    table.insert(tuple.data(), tuple.size());

    std::cout << "1 row inserted\n";
 
}
