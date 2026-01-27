#include "query.h"
#include "../core/db_context.h"
#include "../core/metadata.h"
#include "../parser/parser.h"
#include <iostream>

void execute_create(const std::string& sql) {
    auto args = parse_args(sql);

    if (args[1] == "database")
        create_database(args[2]);

    if (args[1] == "table") {
        Table_info ti = parse_create_table_query(sql);
        ti.table_name = args[2];
        create_table(db_ctx.current_database, ti.table_name, ti.attributes);
        std::cout << "Table created\n";
    }
}
