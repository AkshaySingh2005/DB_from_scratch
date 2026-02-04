#include "pager.h"
#include <filesystem>
#include <iostream>

Pager::Pager(const std::string& filename) : filename(filename), total_pages(0) {
    namespace fs = std::filesystem;
    
    bool exists = fs::exists(filename);
    
    
    file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    
    
    if (!file.is_open() && !exists) {
        std::cout << "Creating new database file: " << filename << std::endl;
        
       
        file.open(filename, std::ios::out | std::ios::binary);
        file.close();
        
       
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    }
    
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open database file: " + filename);
    }
    
    // Get file size and calculate number of pages
    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    total_pages = file_size / PAGE_SIZE;
    file.seekg(0, std::ios::beg);

    
    std::cout << "Opened database: " << filename << " (" << total_pages << " pages)" << std::endl;
}

Pager::~Pager() {
    if (file.is_open()) {
        file.close();
        std::cout << "Closed database: " << filename << std::endl;
    }
}



bool Pager::read_page(uint32_t page_id, Page& page) {
    
    if (page_id >= total_pages) {
        std::cerr << "Error: Page " << page_id << " doesn't exist" << std::endl;
        return false;
    }
    
    // Go to the given id page position and read
    file.seekg(page_id * PAGE_SIZE, std::ios::beg);
    
    if (file.fail()) {
        std::cerr << "Error: Failed to seek to page " << page_id << std::endl;
        return false;
    }
    
    
    file.read(page.get_data(), PAGE_SIZE);
    
    if (file.fail()) {
        std::cerr << "Error: Failed to read page " << page_id << std::endl;
        return false;
    }
    
    std::cout << "✓ Read page " << page_id << " from disk" << std::endl;
    return true;
}

void Pager::write_page(uint32_t page_id, const Page& page) {
    
    // Go to the given id page position and write
    file.seekp(page_id * PAGE_SIZE, std::ios::beg);
    
    if (file.fail()) {
        throw std::runtime_error("Failed to seek to page " + std::to_string(page_id));
    }
    
    
    file.write(page.get_data(), PAGE_SIZE);
    
    if (file.fail()) {
        throw std::runtime_error("Failed to write page " + std::to_string(page_id));
    }
    
    file.flush();
    
    // 
    if (page_id >= total_pages) {
        total_pages = page_id + 1;
    }
    
    std::cout << "✓ Wrote page " << page_id << " to disk" << std::endl;
}

uint32_t Pager::allocate_page() {
    // Create a new empty page
    Page page;
    page.init(total_pages);
    
    // Write it to disk
    write_page(total_pages, page);
    
    return total_pages - 1;  // Note: total_pages already incremented in write_page
}