#pragma once
#include <vector>
#include <string>

std::vector<char> encode_row(const std::vector<std::string>& values);
std::vector<std::string> decode_row(const std::vector<char>& tuple);
