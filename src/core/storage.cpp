#include "storage.h"
#include "metadata.h"
#include <filesystem>
#include <fstream>
#include <cstring>

namespace fs = std::filesystem;

bool insert_values(const std::string& db, const std::string& table, const Table_val& tv) {

    Table_info ti = load_table_metadata(db, table);

    if (tv.values.size() != ti.attributes.size()){
       return false;
    }
        
    fs::path bin = "../data/" + db + "/" + table + "/" + table + ".bin";
    std::ofstream out(bin, std::ios::binary | std::ios::app);

    for (auto& v : tv.values) {
        char buf[256] = {};
        std::strncpy(buf, v.c_str(), 255);
        out.write(buf, 256);
    }
    
    return true;
}
