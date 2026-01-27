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

struct Table_val {
    std::vector<std::string>values;
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

// insert into marks values (1, "Alice", "Math");

Table_val parse_insert_values_query(const std::string& input) {
    Table_val tv;
    
    int id1 = -1, id2 = -1;

    for (int i = 0; i < input.length(); i++) {
        if (input[i] == '(') id1 = i;
        if (input[i] == ')') id2 = i;
    }

    if (id1 == -1 || id2 == -1 || id2 <= id1) {
        throw std::runtime_error("Invalid INSERT syntax");
    }

    std::string temp = input.substr(id1 + 1, id2 - id1 - 1);

    std::stringstream ss(temp);
    std::string token;

    while (std::getline(ss, token, ',')) {
        token = trim(token);

        if (token.front() == '"' && token.back() == '"') {
            token = token.substr(1, token.size() - 2);
        }

        tv.values.push_back(token);
    }



    return tv;
}

Table_info load_table_metadata(
    const std::string& curr_database,
    const std::string& table_name
) {
    fs::path meta_path =
        fs::path("../data") / curr_database / table_name /
        (table_name + ".meta.json");

    if (!fs::exists(meta_path)) {
        throw std::runtime_error("Table does not exist");
    }

    std::ifstream in(meta_path);
    if (!in) {
        throw std::runtime_error("Failed to open metadata");
    }

    json j;
    in >> j;

    Table_info ti;
    ti.table_name = j["table"].get<std::string>();

    for (const auto& col : j["columns"]) {
        ti.attributes.emplace_back(
            col["name"].get<std::string>(),
            col["type"].get<std::string>()
        );
    }

    return ti;
}


bool insert_values(
    const std::string& curr_database,
    const std::string& table_name,
    const Table_val& tv
){
    if (curr_database.empty()) {
        std::cerr << "No database selected\n";
        return false;
    }

    Table_info ti;

    try {
        ti = load_table_metadata(curr_database,table_name);
    }
    catch (...) {
        std::cout<<"load_table_metadata failed" << '\n';
        return false;
    }

    if(tv.values.size() != ti.attributes.size()){
        std::cout<<"Column count mismatch."<<std::endl;
        return false;
    }

    fs::path bin_file = fs::path("../data") / curr_database / table_name / (table_name + ".bin") ;

    std::ofstream out(bin_file,std::ios::binary | std::ios::app);
    if(!out){
        std::cerr << "Failed to open data bin file\n";
        return false;
    }

    for(const auto& val : tv.values){
        char buffer[256] = {0};
        std::strncpy(buffer , val.c_str(),sizeof(buffer) -1);
        out.write(buffer, sizeof(buffer));
    }

    return true;

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

void print_help(){
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

            else if (cmd == ".help") {
                print_help();
            }

            else if(cmd == ".databases"){
                fs::path db_path = fs::path("../data");
                
                for (const auto& dir_entry  : std::filesystem::directory_iterator(db_path)) {
                    
                    if (dir_entry.is_directory()) {
                        std::cout << dir_entry.path().filename().string() << '\n';
                    } 
            
                }                
   
            }

            else if(cmd == ".tables"){
                if (curr_database.empty()) {
                    std::cerr << "No database selected. Use 'use <database>' first.\n";
                    continue;
                }

                fs::path db_path = fs::path("../data")/curr_database;

                if (!fs::exists(db_path)) {
                    std::cerr << "Database directory not found on disk.\n";
                    continue;
                }
                
                bool found = false;

                for (const auto& dir_entry  : std::filesystem::directory_iterator(db_path)) {
                    
                    if (dir_entry.is_directory()) {
                        std::cout << dir_entry.path().filename().string() << '\n';
                        found = true;
                    } 
            
                }     
                
                if(!found){
                    std::cout<<"no tables"<<std::endl;
                }
            }
            continue;
        }

        // SQL COMMANDS
        else if (cmd == "create" && args.size() >= 3) {
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

            else {
                std::cerr << "Unknown create command\n";
            }

        } 

        else if (cmd == "insert") {
            std::cout << "[DEBUG] INSERT command detected\n";
            Table_val tv = parse_insert_values_query(input);
            insert_values(curr_database, args[2], tv);
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
                std::cout << "Using database :  '" << curr_database << "'\n";
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