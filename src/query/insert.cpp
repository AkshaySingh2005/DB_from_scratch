#include "query.h"
#include "../core/db_context.h"
#include "../core/storage.h"
#include "../parser/parser.h"
#include <iostream>

void execute_insert(const std::string& sql) {
    auto args = parse_args(sql);
    std::string table = args[2];

    Table_val tv = parse_insert_values_query(sql);

    if (insert_values(db_ctx.current_database, table, tv))
        std::cout << "1 row inserted\n";
}
