#include "parser.h"
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>

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


// select * from exam ;
// select id,class from exam ;

// select * from marks where id = 2;
// select * from marks where id > 1;
// select name from marks where id <= 2;
// select * from marks where id != 1;


SelectQuery parse_select_query(const std::string &input){
    SelectQuery sq;

    auto v = parse_args(input);

    int id1 = -1 , id2 = -1 , where_pos = -1 ;

    for(int i=0;i<v.size();i++){
        if(v[i] == "select") id1 = i;
        if(v[i] == "from") id2 = i;
        if(v[i] == "where") where_pos = i;
    }

    if(id1 == -1 || id2 == -1 || id2 <= id1){
        throw std::runtime_error("Invalid SELECT syntax");
    }

    sq.table_name = v[id2 + 1];

    if(!sq.table_name.empty() && sq.table_name.back() == ';'){
        sq.table_name.pop_back();
    }
    
    std::string cols;
    for(int i = id1+1 ; i<id2 ; i++){
        cols+=v[i];
    }

    if(cols == "*"){
        sq.columns.push_back("*");
    }
    else{
        std::stringstream ss(cols);
        std::string token;

        while (std::getline(ss, token, ',')) {
            token = trim(token);
            sq.columns.push_back(token);
        }
    }

    if(where_pos != -1){
        if(where_pos + 3 >= v.size()){
            throw std::runtime_error("Invalid WHERE clause");
        }

        sq.where.exits = true;
        sq.where.column = v[where_pos+1];
        sq.where.op     = v[where_pos + 2];
        sq.where.value = v[where_pos+3];
        
        // remove quotes
        if(!sq.where.value.empty() && sq.where.value.front() == '"' && sq.where.value.back() == '"'){
            sq.where.value = sq.where.value.substr(1, sq.where.value.size() - 2);
        }

        if(!sq.where.value.empty() && sq.where.value.back() == ';'){
            sq.where.value.pop_back();
        }

    }
    return sq;
}




