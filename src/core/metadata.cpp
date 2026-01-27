#include "metadata.h"
#include <filesystem>
#include <fstream>
#include "json.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

bool create_database(const std::string& name) {
    return fs::create_directories("../data/" + name);
}

bool drop_database(const std::string& name) {
    namespace fs = std::filesystem;

    if (name.empty() || name == "." || name == "..") {
        return false;
    }

    fs::path db_path = fs::path("../data") / name;

    if (!fs::exists(db_path) || !fs::is_directory(db_path)) {
        return false;
    }

    try {
        fs::remove_all(db_path);
        return true;
    }
    catch (...) {
        return false;
    }
}


bool database_exists(const std::string& name) {
    return fs::exists("../data/" + name);
}

bool create_table(const std::string& db, const std::string& table, const std::vector<std::pair<std::string, std::string>>& attrs) {
    fs::path dir = "../data/" + db + "/" + table;
    fs::create_directories(dir);

    json j;
    j["table"] = table;
    for (auto& a : attrs)
        j["columns"].push_back({{"name", a.first}, {"type", a.second}});

    std::ofstream(dir / (table + ".meta.json")) << j.dump(4);
    std::ofstream(dir / (table + ".bin"), std::ios::binary);
    return true;
}

Table_info load_table_metadata(const std::string& db, const std::string& table) {

    json j; 
    std::ifstream in("../data/" + db + "/" + table + "/" + table + ".meta.json");
    in >> j;

    Table_info ti;
    ti.table_name = j["table"];
    for (auto& c : j["columns"]){
        ti.attributes.emplace_back(c["name"], c["type"]);
    }
        
    return ti;
}
