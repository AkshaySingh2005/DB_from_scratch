#include "query.h"
#include "../core/metadata.h"
#include "../core/db_context.h"
#include "../parser/parser.h"
#include <iostream>

void execute_drop(const std::string& sql) {
    auto args = parse_args(sql);

    if (args.size() < 3 || args[1] != "database") {
        std::cerr << "Invalid DROP syntax. Use:\n" << "drop database <name>\n";
        return;
    }

    if (drop_database(args[2])) {
        if (db_ctx.current_database == args[2]) {
            db_ctx.current_database.clear();
        }
        std::cout << "Database dropped\n";
    } else {
        std::cerr << "Failed to drop database\n";
    }
}
