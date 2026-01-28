#include "query.h"
#include "../core/db_context.h"
#include "../core/metadata.h"
#include "../parser/parser.h"

#include <iostream>

void execute_use(const std::string& sql) {
    auto args = parse_args(sql);

    if (args.size() != 2) {
        std::cerr << "Invalid USE syntax. Use:\n"
                  << "use <database>\n";
        return;
    }

    const std::string& db_name = args[1];

    if (!database_exists(db_name)) {
        std::cerr << "Database does not exist: " << db_name << '\n';
        return;
    }

    db_ctx.current_database = db_name;
    std::cout << "Using database : '" << db_name << "'\n";
}
