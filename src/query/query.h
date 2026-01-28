#pragma once
#include <string>

void execute_create(const std::string& sql);
void execute_insert(const std::string& sql);
void execute_select(const std::string& sql);
void execute_drop(const std::string& sql);
void execute_use(const std::string& sql);
