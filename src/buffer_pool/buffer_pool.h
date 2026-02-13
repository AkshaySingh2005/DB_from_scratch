#pragma once

#include <unordered_map>
#include <list>
#include <cstdint>
#include "../page/page.h"
#include "../page/pager.h"

struct Frame {
    uint32_t page_id;
    Page page;
    bool dirty = false;
};

class BufferPool {
    private:
       Pager& pager;
       size_t capacity;
       
       // Key : Page ID  Val : Frame 
       std::unordered_map<uint32_t,Frame> frames;

       std::list<uint32_t>lru_list;

       void manage_lru_list(uint32_t page_id);
       void delete_page();

    public:
        BufferPool(Pager& pager, size_t capacity);

        Page& fetch_page(uint32_t page_id);
        uint32_t allocate_page();

        void mark_dirty(uint32_t page_id);

        void flush_all();

        Pager& get_pager() { return pager; }


};

