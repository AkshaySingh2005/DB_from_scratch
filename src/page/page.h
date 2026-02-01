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

    int insert_tuple(const void* tuple_data , uint16_t tuple_size){

        PageHeader* h = header();

        if(!can_fit(tuple_size)){
            return -1; //page full
        }
        
        //^  Remember we grow data backwards from the end of the page.
        h->free_space_end -= tuple_size;
        std::memcpy(data + h->free_space_end , tuple_data , tuple_size);

        //^  Creata a new slot
        Slot* slots = get_slot_array();
        uint16_t slot_id = h->num_slots;

        slots[slot_id].offset = h->free_space_end;
        slots[slot_id].length = tuple_size;

        //^  Update header
        h->num_slots++;
        h->free_space_start += sizeof(Slot);


        return slot_id;
        
    }

    std::vector<char> read_tuple(uint16_t slot_id) const {
        const PageHeader* h  = reinterpret_cast<const PageHeader*>(data);

        if(slot_id >= h->num_slots){
            std::cout<<"Invalid slot id"<<std::endl;
        }

        const Slot* slots = reinterpret_cast<const Slot*>(data + sizeof(PageHeader)); 

        const Slot& slot = slots[slot_id];

        if(slot.is_deleted()){
            std::cout<<"slot deleted"<<std::endl;
        }

        std::vector<char>result(slot.length);

        std::memcpy(result.data() , data + slot.offset , slot.length);

        return result;
    }

    bool delete_tuple(uint16_t slot_id) {
        PageHeader* h = header();

       
        if (slot_id >= h->num_slots) {
            return false; 
        }

        Slot* slots = get_slot_array();
        Slot& slot = slots[slot_id];

    
        if (slot.is_deleted()) {
            return false;
        }
        slot.mark_deleted();
        return true;
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






