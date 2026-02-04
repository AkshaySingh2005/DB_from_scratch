#pragma once
#include <fstream>
#include <string>
#include "page.h"

class Pager {
public:
    explicit Pager(const std::string& filename);
    ~Pager();

    Page read_page(uint32_t page_id);
    void write_page(uint32_t page_id, const Page& page);
    uint32_t allocate_page();

private:
    std::fstream file;
    uint32_t total_pages;
};
