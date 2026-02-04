#pragma once

#include "page.h"
#include <fstream>
#include <string>
#include <stdexcept>

class Pager {
private:
    std::fstream file;
    uint32_t total_pages;
    std::string filename;

public:
    Pager(const std::string& filename);
    ~Pager();
    
    // Read a page from disk into the provided Page object
    bool read_page(uint32_t page_id, Page& page);
    
    // Write a page to disk
    void write_page(uint32_t page_id, const Page& page);
    
    // Allocate a new page (returns new page ID)
    uint32_t allocate_page();
    
    // Get total number of pages
    uint32_t get_total_pages() const {
        return total_pages;
    }
};