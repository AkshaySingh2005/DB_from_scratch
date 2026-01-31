#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>

constexpr size_t PAGE_SIZE = 4096 ; // constexpr -> computations at compile time rather than run time.

struct PageHeader {
    uint32_t page_id;
    uint16_t num_slots;
    uint16_t free_space_start;
    uint16_t free_space_end;

    void Init(){
        page_id = 0;
        num_slots = 0;
        free_space_start = sizeof(PageHeader);
        free_space_end = PAGE_SIZE;
    }
};



struct Slot {
    uint16_t offset;
    uint16_t length;

    bool is_deleted() const {
        return offset == 0 && length == 0;
    }


    void mark_deleted(){
        offset = 0;
        length = 0;
    }
};


class Page {
private:
    char data[PAGE_SIZE];

    PageHeader* header(){
        return reinterpret_cast<PageHeader*>(data);
    }

    Slot* get_slot_array(){
        return reinterpret_cast<Slot*>(data + sizeof(PageHeader));
    }

public:
    
    Page(){
        memset(data,0,PAGE_SIZE);

        header()->Init();

    }

    void init(uint32_t page_id){

        memset(data,0,PAGE_SIZE);

        PageHeader* h = header();

        h->page_id = page_id;
        h->num_slots = 0;
        h->free_space_start = sizeof(PageHeader);
        h->free_space_end = PAGE_SIZE;
    }

    size_t get_free_space() const {
        const PageHeader* h = reinterpret_cast<const PageHeader*>(data);

        if(h->free_space_end <= h->free_space_start){
            return 0; // Page is full
        }

        return h->free_space_end - h->free_space_start;
    }

    bool can_fit(size_t tuple_size) const {
        return get_free_space() >= (sizeof(Slot) + tuple_size);
    }

    void print_info() const {
        const PageHeader* h = reinterpret_cast<const PageHeader*>(data);
        
        std::cout << "Page ID: " << h->page_id << std::endl;
        std::cout << "Num Slots: " << h->num_slots << std::endl;
        std::cout << "Free Space Start: " << h->free_space_start << std::endl;
        std::cout << "Free Space End: " << h->free_space_end << std::endl;
        std::cout << "Free Space Available: " << get_free_space() << " bytes" << std::endl;
    }

};






