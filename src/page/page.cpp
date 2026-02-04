#include "page.h"
#include <cstring>
#include <iostream>


void PageHeader::Init() {
    page_id = 0;
    num_slots = 0;
    free_space_start = sizeof(PageHeader);
    free_space_end = PAGE_SIZE;
}


bool Slot::is_deleted() const {
    return offset == 0 && length == 0;
}

void Slot::mark_deleted() {
    offset = 0;
    length = 0;
}


Page::Page() {
    memset(data, 0, PAGE_SIZE);
    header()->Init();
}

PageHeader* Page::header() {
    return reinterpret_cast<PageHeader*>(data);
}

const PageHeader* Page::header() const {
    return reinterpret_cast<const PageHeader*>(data);
}

Slot* Page::get_slot_array() {
    return reinterpret_cast<Slot*>(data + sizeof(PageHeader));
}

const Slot* Page::get_slot_array() const {
    return reinterpret_cast<const Slot*>(data + sizeof(PageHeader));
}

void Page::init(uint32_t page_id) {
    memset(data, 0, PAGE_SIZE);
    PageHeader* h = header();
    h->page_id = page_id;
    h->num_slots = 0;
    h->free_space_start = sizeof(PageHeader);
    h->free_space_end = PAGE_SIZE;
}

size_t Page::get_free_space() const {
    const PageHeader* h = header();
    if (h->free_space_end <= h->free_space_start) return 0;
    return h->free_space_end - h->free_space_start;
}

bool Page::can_fit(size_t tuple_size) const {
    return get_free_space() >= (sizeof(Slot) + tuple_size);
}

int Page::insert_tuple(const void* tuple_data, uint16_t tuple_size) {
    PageHeader* h = header();
    Slot* slots = get_slot_array();

    if (!can_fit(tuple_size)) return -1;

    // Try to reuse a deleted slot
    for (uint16_t i = 0; i < h->num_slots; i++) {
        if (slots[i].is_deleted()) {
            h->free_space_end -= tuple_size;
            memcpy(data + h->free_space_end, tuple_data, tuple_size);
            slots[i].offset = h->free_space_end;
            slots[i].length = tuple_size;
            return i;
        }
    }

    
    h->free_space_end -= tuple_size;
    memcpy(data + h->free_space_end, tuple_data, tuple_size);

    uint16_t slot_id = h->num_slots;
    slots[slot_id].offset = h->free_space_end;
    slots[slot_id].length = tuple_size;
    h->num_slots++;
    h->free_space_start += sizeof(Slot);

    return slot_id;
}

std::vector<char> Page::read_tuple(uint16_t slot_id) const {
    const PageHeader* h = header();
    if (slot_id >= h->num_slots) return {};

    const Slot* slots = get_slot_array();
    const Slot& slot = slots[slot_id];
    if (slot.is_deleted()) return {};

    std::vector<char> result(slot.length);
    memcpy(result.data(), data + slot.offset, slot.length);
    return result;
}

bool Page::delete_tuple(uint16_t slot_id) {
    PageHeader* h = header();
    if (slot_id >= h->num_slots) return false;

    Slot* slots = get_slot_array();
    if (slots[slot_id].is_deleted()) return false;

    slots[slot_id].mark_deleted();
    return true;
}

void Page::cleanup_garbage() {
    PageHeader* h = header();
    Slot* slots = get_slot_array();
    uint16_t write_ptr = PAGE_SIZE;

    for (uint16_t i = 0; i < h->num_slots; i++) {
        if (slots[i].is_deleted()) continue;
        
        write_ptr -= slots[i].length;
        memmove(data + write_ptr, data + slots[i].offset, slots[i].length);
        slots[i].offset = write_ptr;
    }

    h->free_space_end = write_ptr;
    h->free_space_start = sizeof(PageHeader) + (h->num_slots * sizeof(Slot));
}

void Page::print_info() const {
    const PageHeader* h = header();
    std::cout << "Page ID: " << h->page_id 
              << " | Slots: " << h->num_slots 
              << " | Free: " << get_free_space() << " bytes" << std::endl;
}

