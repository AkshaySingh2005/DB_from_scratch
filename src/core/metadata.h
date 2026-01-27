#pragma once
#include <string>
#include "../parser/parser.h"

bool create_database(const std::string& name);
bool drop_database(const std::string& name);   
bool database_exists(const std::string& name);

bool create_table(
    const std::string& db,
    const std::string& table,
    const std::vector<std::pair<std::string, std::string>>& attrs
);

Table_info load_table_metadata(
    const std::string& db,
    const std::string& table
);
