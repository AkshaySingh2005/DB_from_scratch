
#pragma once
#include <string>

struct DatabaseContext {
    std::string current_database;
};

extern DatabaseContext db_ctx;
