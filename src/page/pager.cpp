#include "pager.h"
#include <filesystem>

Pager::Pager(const std::string& filename){
    namespace fs = std::filesystem;
    
    bool exists = fs::exists(filename);

    file.open(filename, std::ios::in | std::ios::out | std::ios::binary);

    if(!file && !exists){
        file.open(filename, std::ios::out | std::ios::binary);
        file.close();
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    }

    file.seekg(0,std::ios::end);
    total_pages = file.tellg() / PAGE_SIZE;
}

Pager::~Pager() {
    file.close();
}

Page Pager::read_page(uint32_t page_id){
   if(page_id >= total_pages){
      throw std::runtime_error("Page does not exist");
   }

   Page page;
   file.seekg(page_id * PAGE_SIZE); // N * 4096 will that id page 
   file.read(reinterpret_cast<char*>(&page), PAGE_SIZE);

   return page;

}

void Pager::write_page(uint32_t page_id , const Page& page){
    file.seekp(page_id * PAGE_SIZE);
     file.write(reinterpret_cast<const char*>(&page), PAGE_SIZE);
     file.flush();
}

uint32_t Pager::allocate_page(){
    Page page;
    page.init(total_pages);

    write_page(total_pages , page);

    return total_pages++;
}

