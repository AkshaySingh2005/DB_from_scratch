#include "table.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include "json.hpp"

using json = nlohmann::json;

namespace fs = std::filesystem;


bool create_table(
    const std::string& curr_database,
    const std::string& table_name,
    const std::vector<std::pair<std::string, std::string>>& attributes
) {
    if (curr_database.empty()) {
        std::cerr << "No database selected\n";
        return false;
    }

    fs::path table_dir = fs::path("../data") / curr_database / table_name;

    if (fs::exists(table_dir)) {
        std::cerr << "Table already exists\n";
        return false;
    }

    try {
        fs::create_directories(table_dir);
        
        // json handling 
        fs::path meta_file = table_dir / (table_name + ".meta.json");

        json j;
        j["table"] = table_name;
        j["columns"] = json::array();

        for (const auto& [name, type] : attributes) {
            j["columns"].push_back({
                {"name", name},
                {"type", type}
            });
        }

        std::ofstream meta(meta_file);
        if (!meta) throw std::runtime_error("Meta write failed");
        meta << j.dump(4)<<'\n';
        
        // bin handling 
        fs::path data_file = table_dir / (table_name + ".bin");

        std::ofstream data(data_file, std::ios::binary);

        if (!data) throw std::runtime_error("Data file failed");

        std::cout << "Table created: " << table_name << '\n';
        return true;
    }
    catch (...) {
        fs::remove_all(table_dir);
        return false;
    }
}









