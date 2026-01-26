#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <readline/readline.h>
#include <readline/history.h>
#include "json.hpp"
#include "table.h"

using json = nlohmann::json;

namespace fs = std::filesystem;

std::string curr_database;

struct Table_info {
    std::string table_name;
    std::vector<std::pair<std::string, std::string>> attributes;
};


std::string trim(std::string s) {
    s.erase(0, s.find_first_not_of(" \t\n"));
    s.erase(s.find_last_not_of(" \t\n") + 1);
    return s;
}



std::vector<std::string> parse_args(const std::string &input){
    std::stringstream ss(input);
    std::vector<std::string>tokens;
    std::string token;

    while(ss >> token){
        tokens.push_back(token);
    }

    return tokens;  
}

// create table marks ( id INTEGER, email TEXT, phone TEXT ) ; 

Table_info  parse_create_table_query(const std::string & input){
    
    Table_info ti;
 
    int id1 = 0 , id2 = 0;

    for(int i=0;i<input.length();i++){
        if(input[i] == '('){
           id1 = i;
        }
    }

    for(int i=0;i<input.length();i++){
        if(input[i] == ')'){
           id2 = i;
        }
    }
    
    std::string temp ;

    for(int i=id1+1 ; i<id2 ; i++){
        temp+=input[i];
    }

    std::string token;
    std::vector<std::string> tokens;
    std::stringstream ss(temp);

    while (std::getline(ss, token, ',')) {
        tokens.push_back(token);
    }

    for(const auto& it : tokens){
        std::istringstream item(it);
        std::string name , type ;
        item >> name >> type;
        name = trim(name);
        type = trim(type);

        if (!name.empty() && !type.empty()) {
            ti.attributes.emplace_back(name, type);
        }
    }

    // for (const auto& pair : ti.attributes) {
    //     std::cout<< pair.first << "," << pair.second<< std::endl;
    // }

    return ti;

}


bool create_database(const std::string& db_name) {
    fs::path db_path = fs::path("../data") / db_name;

    try {
        if (fs::exists(db_path)) return false;
        fs::create_directories(db_path);
        return true;
    } catch (...) {
        return false;
    }
}

bool drop_database(const std::string& db_name) {
    if (db_name.empty() || db_name == "." || db_name == "..")
        return false;

    fs::path db_path = fs::path("../data") / db_name;

    try {
        if (!fs::exists(db_path)) return false;
        fs::remove_all(db_path);
        return true;
    } catch (...) {
        return false;
    }
}

bool database_exists(const std::string& db_name) {
    fs::path db_path = fs::path("../data") / db_name;

    try {
        return fs::exists(db_path) && fs::is_directory(db_path);
    } catch (...) {
        return false;
    }
}

// bool create_table(const std::string& table_name){
//     if (curr_database.empty()) {
//         std::cerr << "No database selected. Use 'use <database>' first.\n";
//         return false;
//     }

//     fs::path table_dir = fs::path("../data") / curr_database / table_name;

//     if (fs::exists(table_dir)) {
//         std::cerr << "Table already exists: " << table_name << '\n';
//         return false;
//     }

//     try {
//         fs::create_directories(table_dir);

//         fs::path meta_file = table_dir / (table_name + ".meta.json");
//         std::ofstream meta(meta_file);
//         if (!meta) throw std::runtime_error("Failed to create meta file");

//         meta << "{\n"
//              << "  \"table\": \"" << table_name << "\",\n"
//              << "  \"columns\": []\n"
//              << "}\n";
//         meta.close();

//         fs::path data_file = table_dir / (table_name + ".bin");
//         std::ofstream data(data_file, std::ios::binary);
//         if (!data) throw std::runtime_error("Failed to create data file");

//         data.close();

//         std::cout << "Table created: " << table_name << '\n';
//         return true;
//     }
//     catch (const std::exception& e) {
//         fs::remove_all(table_dir); // rollback
//         std::cerr << "Create table failed: " << e.what() << '\n';
//         return false;
//     }
// }






int main(){
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    std::cout<<"Enter .help for usage hints."<<std::endl;


   while (true) {
        char* raw = readline("sqlite> ");
        if (!raw) break;

        std::string input(raw);
        free(raw);

        auto args = parse_args(input);
        if (args.empty()) continue;

        const std::string& cmd = args[0];

        Table_info ti;

        // DOT COMMANDS
        if (cmd[0] == '.') {
            if (cmd == ".exit") break;

            if (cmd == ".help") {
                std::cout << std::left << std::setw(15) << ".exit" << std::left << std::setw(10) <<"Exit this program with return-code CODE"<< std::endl;
            }
            continue;
        }

        // SQL COMMANDS
        if (cmd == "create" && args.size() >= 3) {
            if (args[1] == "database") {
                if (create_database(args[2])) {
                    std::cout << "Database created : "<<args[2]<<'\n';
                } else {
                    std::cout << "Error : Database already exists : " <<args[2]<<'\n';
                }
            }
            else if (args[1] == "table") {

                Table_info ti = parse_create_table_query(input);

                ti.table_name = args[2];

                create_table(curr_database, ti.table_name, ti.attributes);
            }
        } 
        
        else if (cmd == "drop" && args.size() >= 3 && args[1] == "database") {
            if (drop_database(args[2])) {
                std::cout << "Database dropped\n";
            } else {
                std::cout << "Failed to drop database\n";
            }
        }

        else if(cmd == "use" && args.size() >=2 ){
            if(database_exists(args[1])){
                curr_database = args[1];
                std::cout << "Using database '" << curr_database << "'\n";
            }
            else if(!database_exists(args[1])){
                std::cerr << "Database does not exist: " << args[1] << '\n';
            }
            
        }

        
        
        else {
            std::cerr << "Unknown command\n";
        }
    }




}