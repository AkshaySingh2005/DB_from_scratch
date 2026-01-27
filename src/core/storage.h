#pragma once

#include "../parser/parser.h"
#include <string>

bool insert_values(
    const std::string& db,
    const std::string& table,
    const Table_val& tv
);
