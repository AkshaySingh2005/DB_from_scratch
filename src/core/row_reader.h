#pragma once

#include <string>
#include <vector>
#include "metadata.h"

struct Row {
    std::vector<std::string> values;
};

std::vector<Row> read_all_rows(
    const std::string& db,
    const Table_info& table
);
