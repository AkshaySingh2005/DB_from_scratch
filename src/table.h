#pragma once

#include <string>
#include <vector>


bool create_table(const std::string& curr_database , const std::string& table_name , const std::vector<std::pair<std::string, std::string>> &attributes);