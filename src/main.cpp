#include <iostream>
#include <iomanip>
#include <filesystem>

#include <readline/readline.h>
#include <readline/history.h>

#include "core/db_context.h"
#include "parser/parser.h"
#include "core/metadata.h"
#include "core/storage.h"

namespace fs = std::filesystem;



void print_help() {
    std::cout
        << std::left << std::setw(15) << ".exit"
        << "Exit this program\n"

        << std::left << std::setw(15) << ".help"
        << "Show this help message\n"

        << std::left << std::setw(15) << ".databases"
        << "List all databases\n"

        << std::left << std::setw(15) << ".tables"
        << "List tables in the current database\n";
}



int main() {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    std::cout << "Enter .help for usage hints.\n";

    while (true) {
        char* raw = readline("sqlite> ");
        if (!raw) break;

        std::string input(raw);
        free(raw);

        auto args = parse_args(input);
        if (args.empty()) continue;

        const std::string& cmd = args[0];

        // DOT COMMANDS //

        if (cmd[0] == '.') {

            if (cmd == ".exit") break;

            else if (cmd == ".help") {
                print_help();
            }

            else if (cmd == ".databases") {
                fs::path db_root = "../data";

                if (!fs::exists(db_root)) {
                    std::cout << "No databases\n";
                    continue;
                }

                for (const auto& entry : fs::directory_iterator(db_root)) {
                    if (entry.is_directory()) {
                        std::cout << entry.path().filename().string() << '\n';
                    }
                }
            }

            else if (cmd == ".tables") {
                if (db_ctx.current_database.empty()) {
                    std::cerr << "No database selected. Use 'use <database>' first.\n";
                    continue;
                }

                fs::path db_path = fs::path("../data") / db_ctx.current_database;

                if (!fs::exists(db_path)) {
                    std::cerr << "Database directory not found on disk.\n";
                    continue;
                }

                bool found = false;
                for (const auto& entry : fs::directory_iterator(db_path)) {
                    if (entry.is_directory()) {
                        std::cout << entry.path().filename().string() << '\n';
                        found = true;
                    }
                }

                if (!found) {
                    std::cout << "no tables\n";
                }
            }

            else {
                std::cerr << "Unknown dot command\n";
            }

            continue;
        }

        // SQL COMMANDS //

        else if (cmd == "use" && args.size() >= 2) {
            if (database_exists(args[1])) {
                db_ctx.current_database = args[1];
                std::cout << "Using database : '" << db_ctx.current_database << "'\n";
            } else {
                std::cerr << "Database does not exist: " << args[1] << '\n';
            }
        }

        else if (cmd == "create" && args.size() >= 3) {

            if (args[1] == "database") {
                if (create_database(args[2])) {
                    std::cout << "Database created: " << args[2] << '\n';
                } else {
                    std::cerr << "Database already exists: " << args[2] << '\n';
                }
            }

            else if (args[1] == "table") {
                if (db_ctx.current_database.empty()) {
                    std::cerr << "No database selected\n";
                    continue;
                }

                Table_info ti = parse_create_table_query(input);
                ti.table_name = args[2];

                if (create_table(db_ctx.current_database, ti.table_name, ti.attributes)) {
                    std::cout << "Table created: " << ti.table_name << '\n';
                }
            }

            else {
                std::cerr << "Unknown CREATE command\n";
            }
        }

        else if (cmd == "insert") {

            if (args.size() < 3 || args[1] != "into" || args[3] != "values" ) {
                std::cerr << "Invalid INSERT syntax\n";
                continue;
            }

            if (db_ctx.current_database.empty()) {
                std::cerr << "No database selected\n";
                continue;
            }

            std::string table_name = args[2];

            Table_val tv = parse_insert_values_query(input);

            if (insert_values(db_ctx.current_database, table_name, tv)) {
                std::cout << "1 row inserted\n";
            } 
            else {
                std::cerr << "Insert failed\n";
            }
        }

        else if (cmd == "drop" && args.size() >= 3 && args[1] == "database") {
            if (drop_database(args[2])) {
                std::cout << "Database dropped\n";
            } 
            else {
                std::cerr << "Failed to drop database\n";
            }
        }

        else {
            std::cerr << "Unknown command\n";
        }
    }

    return 0;
}
