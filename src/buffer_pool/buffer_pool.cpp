#include "buffer_pool.h"

BufferPool::BufferPool(Pager& pager, size_t capacity) : pager(pager), capacity(capacity) {}


// managing the LRU list 
void BufferPool::manage_lru_list(uint32_t page_id){
    lru_list.remove(page_id);
    lru_list.push_front(page_id); // recently used 
}

void BufferPool::delete_page(){
    // Frames not full still have capacity to hold pages 
    if(frames.size() < capacity){
        return;
    }

    uint32_t victim = lru_list.back();

    lru_list.pop_back();

    Frame& frame = frames[victim];

    if(frame.dirty){
        pager.write_page(frame.page_id , frame.page);
    }

    frames.erase(victim);
}

Page& BufferPool::fetch_page(uint32_t page_id){
    
    // already in buffer
    auto it = frames.find(page_id);

    if(it != frames.end()){
        manage_lru_list(page_id);
        return it->second.page;
    }

    delete_page();
    
    // new page 
    Frame frame;
    frame.page_id = page_id;
    frame.dirty = false;

    if (!pager.read_page(page_id, frame.page)) {
        throw std::runtime_error("Failed to read page");
    }

    frames[page_id] = frame;
    lru_list.push_front(page_id);

    return frames[page_id].page;

}

// new page directly goes in memory 

uint32_t BufferPool::allocate_page() {

    delete_page();

    uint32_t page_id = pager.allocate_page();

    Frame frame;
    frame.page_id = page_id;
    frame.dirty = true; // For now it exist in memory only not in disk
    frame.page.init(page_id);

    frames[page_id] = frame;
    lru_list.push_front(page_id);

    return page_id;
}

void BufferPool::mark_dirty(uint32_t page_id){
    auto it = frames.find(page_id);
    if(it != frames.end()){
        it->second.dirty = true;
    }
}


// Write all dirty pages to disk
void BufferPool::flush_all(){
    for(auto& [page_id , frame] : frames){
        if(frame.dirty){
            pager.write_page(page_id , frame.page); // now write to memory
            frame.dirty = false;
        }
    }
}













