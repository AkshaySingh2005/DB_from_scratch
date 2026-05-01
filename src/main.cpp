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
#include "buffer_pool/buffer_pool.h"



namespace fs = std::filesystem;

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

using namespace ftxui;

#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <string>



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
    // 1. The "State" (Equivalent to React's useState)
    std::string sql_query = "SELECT * FROM users;";
    std::string result_text = "Waiting for query...";

    // 2. The Interactive Components
    // We bind the input box directly to our state variable
    Component input_box = Input(&sql_query, "Type your SQL here...");
    
    // The button takes a label and an "onClick" lambda function
    Component run_button = Button("Run Query", [&] {
        // When clicked, we update the state.
        // In a real app, you would pass sql_query to your DB engine here.
        result_text = "Executing: " + sql_query + "\n[Success] 5 rows returned.";
    });

    // We must group interactive elements in a Container so FTXUI knows 
    // how to move focus between them using the arrow keys/Tab.
    Component interactivity_container = Container::Vertical({
        input_box,
        run_button
    });

    // 3. The Renderer (The UI Layout)
    // We pass our container in, and define how everything visually fits together.
    Component final_ui = Renderer(interactivity_container, [&] {
        return vbox({
            text(" Database Query Engine ") | bold | hcenter,
            separator(),
            
            // The Input Area
            hbox({
                text(" SQL > ") | color(Color::Yellow),
                input_box->Render() | flex, // Render the input component
            }),
            
            // The Button Area
            run_button->Render() | hcenter,
            separator(),
            
            // The Results Area
            text(" Output:") | bold,
            text(result_text) | dim | border
            
        }) | border;
    });

    // 4. Mount and Run
    auto screen = ScreenInteractive::TerminalOutput();
    
    // This blocks the main thread and runs the UI loop until you press Ctrl+C
    screen.Loop(final_ui);

    return 0;
}

//int main() {

    // std::cout << std::unitbuf;
    // std::cerr << std::unitbuf;




    // setup_readline();

    // std::cout << "Enter .help for usage hints.\n";

    // while (true) {
    //     char* raw = readline("sqlite> ");
    //     if (!raw) break;

    //     std::string input(raw);
    //     free(raw);

    //     if(!input.empty()){
    //         add_history(input.c_str());
    //     }

    //     if(input == "page"){
    //         Pager pager("heap1.db");
    //         BufferPool buffer(pager, 2);
    //         HeapTable table(buffer);

    //         std::vector<std::string> row1 = {"1", "Alice", "Math"};
    //         std::vector<std::string> row2 = {"2", "Bob", "Science"};
    //         std::vector<std::string> row3 = {"3", "Charlie", "English"};

    //         auto t1 = encode_row(row1);
    //         auto t2 = encode_row(row2);
    //         auto t3 = encode_row(row3);

    //         table.insert(t1.data(), t1.size());
    //         table.insert(t2.data(), t2.size());
    //         table.insert(t3.data(), t3.size());

    //         auto rows = table.scan_all();

    //         for (auto& r : rows) {
    //             std::cout << std::string(r.begin(), r.end()) << "\n";
    //         }

    //         buffer.flush_all();

            

    //         // Pager pager("buffer.db");
    //         // HeapTable table(pager);

    //         // std::vector<std::string> row1 = {"1", "Alice", "Math"};
    //         // std::vector<std::string> row2 = {"2", "Bob", "Science"};
    //         // std::vector<std::string> row3 = {"3", "Charlie", "English"};

    //         // auto t1 = encode_row(row1);
    //         // auto t2 = encode_row(row2);
    //         // auto t3 = encode_row(row3);

    //         // table.insert(t1.data(), t1.size());
    //         // table.insert(t2.data(), t2.size());
    //         // table.insert(t3.data(), t3.size());

    //         // Page debug;
    //         // pager.read_page(0, debug);
    //         // debug.print_info();

    //         // // auto rows = table.scan_all();
    //         // auto rows = table.scan_with_rid();

    //         // for (auto& r : rows) {
    //         //     std::cout << "RID: (" 
    //         //             << r.rid.page_id << ", "
    //         //             << r.rid.slot_id << ") -> ";

    //         //     auto values = decode_row(r.data);
    //         //     for (auto& v : values)
    //         //         std::cout << v << " ";

    //         //     std::cout << "\n";
    //         // }
    //     }

    //     // DOT COMMANDS //

    //     if (input[0] == '.') {

    //         if (input == ".exit") {
    //             if (db_ctx.buffer) {
    //                 db_ctx.buffer->flush_all();
    //             }
    //             break;
    //         }

    //         else if(input.substr(0, 6) == ".clear"){
    //             // Just use system clear - it's the most reliable
    //             #ifdef _WIN32
    //                 system("cls");
    //             #else
    //                 system("clear");
    //             #endif
    //             continue;
    //         }
            
    //         else if (input == ".help") {
    //             print_help();
    //         }

    //         else if (input == ".databases") {
    //             fs::path db_root = "../data";

    //             if (!fs::exists(db_root)) {
    //                 std::cout << "No databases\n";
    //                 continue;
    //             }

    //             for (const auto& entry : fs::directory_iterator(db_root)) {
    //                 if (entry.is_directory()) {
    //                     std::cout << entry.path().filename().string() << '\n';
    //                 }
    //             }
    //         }

    //         else if (input == ".tables") {
    //             if (db_ctx.current_database.empty()) {
    //                 std::cerr << "No database selected. Use 'use <database>' first.\n";
    //                 continue;
    //             }

    //             fs::path db_path = fs::path("../data") / db_ctx.current_database;

    //             if (!fs::exists(db_path)) {
    //                 std::cerr << "Database directory not found on disk.\n";
    //                 continue;
    //             }

    //             bool found = false;
    //             for (const auto& entry : fs::directory_iterator(db_path)) {
    //                 if (entry.is_directory()) {
    //                     std::cout << entry.path().filename().string() << '\n';
    //                     found = true;
    //                 }
    //             }

    //             if (!found) {
    //                 std::cout << "no tables\n";
    //             }
    //         }

    //         else if (input.rfind(".history", 0) == 0) {
    //             history_cmd(input);
    //         }

    //         else {
    //             std::cerr << "Unknown dot command\n";
    //         }

    //         continue;
    //     }

    //     // SQL COMMANDS //

    //     auto first_space = input.find(' ');
    //     std::string cmd = (first_space == std::string::npos) ? input : input.substr(0, first_space);


    //     if (cmd == "use") {
    //         execute_use(input);
    //     }
    //     else if (cmd == "create") {
    //         execute_create(input);
    //     }
    //     else if (cmd == "insert") {
    //         execute_insert(input);
    //     }
    //     else if (cmd == "select") {
    //         execute_select(input);
    //     }
    //     else if (cmd == "drop") {
    //         execute_drop(input);
    //     }
    //     else if (cmd == "delete"){
    //         execute_delete(input);
    //     }
    //     else if (cmd == "update"){
    //         execute_update(input);
    //     }
    //     else {
    //         std::cerr << "Unknown command\n";
    //     }
    // }

    // if (db_ctx.buffer) {
    //     db_ctx.buffer->flush_all();
    // }

 
//}
