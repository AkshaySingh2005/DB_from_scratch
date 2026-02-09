#pragma once

#include <string>
#include <vector>

struct Table_info {
    std::string table_name;
    std::vector<std::pair<std::string, std::string>> attributes;
};

struct Table_val {
    std::vector<std::string> values;
};

struct WhereClause {
    bool exits = false;
    std::string column;
    std::string op;
    std::string value;
};

struct SelectQuery {
    std::string table_name;
    std::vector<std::string> columns;
    WhereClause where ; 
};


std::vector<std::string> parse_args(const std::string& input);

Table_info parse_create_table_query(const std::string& input);
Table_val  parse_insert_values_query(const std::string& input);
SelectQuery parse_select_query(const std::string& input);

std::string trim(const std::string& s);
