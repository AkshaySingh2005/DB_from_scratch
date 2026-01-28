#include "query.h"
#include "../core/db_context.h"
#include "../core/metadata.h"
#include "../parser/parser.h"
#include <iostream>

void execute_create(const std::string& sql) {
    auto args = parse_args(sql);

    if (args.size() < 3) {
        std::cerr << "Invalid CREATE syntax\n";
        return;
    }

    if (args[1] == "database")  {
        
        if (args.size() != 3) {
            std::cerr << "Invalid CREATE DATABASE syntax.\n"
                      << "Use: create database <name>\n";
            return;
        }
        std::string db_name = args[2];

        if (create_database(db_name)) {
            std::cout << "Database created: " << db_name << '\n';
        } else {
            std::cerr << "Database already exists: " << db_name << '\n';
        }

        return;
    }
        
    if (args[1] == "table") {
        if (db_ctx.current_database.empty()) {
            std::cerr << "No database selected\n";
            return;
        }

        if (args.size() < 3) {
            std::cerr << "Invalid CREATE TABLE syntax\n";
            return;
        }

        Table_info ti = parse_create_table_query(sql);
        ti.table_name = args[2];

        if (create_table(db_ctx.current_database, ti.table_name, ti.attributes)) {
            std::cout << "Table created: " << ti.table_name << '\n';
        }
        return;
    }

    std::cerr << "Unknown CREATE command\n";
}
