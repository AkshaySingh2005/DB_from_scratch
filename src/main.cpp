#include <iostream>
#include <iomanip>
#include <filesystem>

#include <readline/readline.h>
#include <readline/history.h>

#include "context/db_context.h"
#include "query/query.h"
#include "gnu_readline/completion.h"
#include "page/page.h"
#include "page/pager.h"
#include "heap_table/heap_table.h"
#include "core/tuple_encoder.h"


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




    setup_readline();

    std::cout << "Enter .help for usage hints.\n";

    while (true) {
        char* raw = readline("sqlite> ");
        if (!raw) break;

        std::string input(raw);
        free(raw);

        if(!input.empty()){
            add_history(input.c_str());
        }

        if(input == "page"){
            Pager pager("heap.db");
            HeapTable table(pager);

            std::vector<std::string> row1 = {"1", "Alice", "Math"};
            std::vector<std::string> row2 = {"2", "Bob", "Science"};
            std::vector<std::string> row3 = {"3", "Charlie", "English"};

            auto t1 = encode_row(row1);
            auto t2 = encode_row(row2);
            auto t3 = encode_row(row3);

            table.insert(t1.data(), t1.size());
            table.insert(t2.data(), t2.size());
            table.insert(t3.data(), t3.size());

            Page debug;
            pager.read_page(0, debug);
            debug.print_info();

            // auto rows = table.scan_all();
            auto rows = table.scan_with_rid();

            for (auto& r : rows) {
                std::cout << "RID: (" 
                        << r.rid.page_id << ", "
                        << r.rid.slot_id << ") -> ";

                auto values = decode_row(r.data);
                for (auto& v : values)
                    std::cout << v << " ";

                std::cout << "\n";
            }
        }

        // DOT COMMANDS //

        if (input[0] == '.') {

            if (input == ".exit") break;

            else if (input == ".help") {
                print_help();
            }

            else if (input == ".databases") {
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

            else if (input == ".tables") {
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

            else if (input.rfind(".history", 0) == 0) {
                history_cmd(input);
            }

            else {
                std::cerr << "Unknown dot command\n";
            }

            continue;
        }

        // SQL COMMANDS //

        auto first_space = input.find(' ');
        std::string cmd = (first_space == std::string::npos) ? input : input.substr(0, first_space);


        if (cmd == "use") {
            execute_use(input);
        }
        else if (cmd == "create") {
            execute_create(input);
        }
        else if (cmd == "insert") {
            execute_insert(input);
        }
        else if (cmd == "select") {
            execute_select(input);
        }
        else if (cmd == "drop") {
            execute_drop(input);
        }
        else if (cmd == "delete"){
            execute_delete(input);
        }
        else if (cmd == "update"){
            execute_update(input);
        }
        else {
            std::cerr << "Unknown command\n";
        }
    }

    return 0;
}
