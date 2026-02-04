#pragma once

#include <cstdint>
#include <vector>

constexpr size_t PAGE_SIZE = 4096; 

//Page Meta data
struct PageHeader {
    uint32_t page_id;
    uint16_t num_slots;
    uint16_t free_space_start;
    uint16_t free_space_end;

    void Init();
};

// Slot points to stored data/tuple 
struct Slot {
    uint16_t offset;
    uint16_t length;

    bool is_deleted() const;
    void mark_deleted();
};

class Page {
private:
    char data[PAGE_SIZE];

    // Internal helpers
    PageHeader* header();

    const PageHeader* header() const;

    Slot* get_slot_array();

    const Slot* get_slot_array() const;


public:
    Page();

    char* get_data();
    const char* get_data() const;

    
    void init(uint32_t page_id);

    size_t get_free_space() const;

    bool can_fit(size_t tuple_size) const;
    
    int insert_tuple(const void* tuple_data, uint16_t tuple_size);

    std::vector<char> read_tuple(uint16_t slot_id) const;

    bool delete_tuple(uint16_t slot_id);

    uint16_t get_num_slots() const;
    
    void cleanup_garbage();

    void print_info() const;
};