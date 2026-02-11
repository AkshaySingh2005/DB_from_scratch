#pragma once
#include <string>

inline bool eval_condn(const std::string& lhs , const std::string& op , const std::string& rhs){
    if(op == "="){
        return lhs==rhs;
    }

    if(op == "!=" || op == "<>"){
        return lhs!=rhs;
    }

    double left = std::stod(lhs);
    double right = std::stod(rhs);

    if (op == "<")
        return left < right;

    if (op == ">")
        return left > right;

    if (op == "<=")
        return left <= right;

    if (op == ">=")
        return left >= right;

    return false;

}

void execute_create(const std::string& sql);
void execute_insert(const std::string& sql);
void execute_select(const std::string& sql);
void execute_drop(const std::string& sql);
void execute_use(const std::string& sql);
void execute_delete(const std::string sql);
void execute_update(const std::string sql);
