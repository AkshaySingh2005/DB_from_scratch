#include "parser.h"
#include <sstream>
#include <algorithm>

std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t");
    size_t end   = s.find_last_not_of(" \t");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

std::vector<std::string> parse_args(const std::string& input) {
    std::stringstream ss(input);
    std::vector<std::string> tokens;
    std::string tok;
    while (ss >> tok) tokens.push_back(tok);
    return tokens;
}

Table_info parse_create_table_query(const std::string& input) {
    Table_info ti;

    size_t l = input.find('(');
    size_t r = input.find(')');

    std::string cols = input.substr(l + 1, r - l - 1);
    std::stringstream ss(cols);
    std::string token;

    while (std::getline(ss, token, ',')) {
        std::stringstream col(token);
        std::string name, type;
        col >> name >> type;
        ti.attributes.emplace_back(trim(name), trim(type));
    }
    return ti;
}

Table_val parse_insert_values_query(const std::string& input) {
    Table_val tv;

    size_t l = input.find('(');
    size_t r = input.find(')');

    std::string vals = input.substr(l + 1, r - l - 1);
    std::stringstream ss(vals);
    std::string token;

    while (std::getline(ss, token, ',')) {
        token = trim(token);
        if (token.front() == '"' && token.back() == '"')
            token = token.substr(1, token.size() - 2);
        tv.values.push_back(token);
    }
    return tv;
}
