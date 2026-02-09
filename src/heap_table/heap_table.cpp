#include "heap_table.h"
#include <iostream>

HeapTable::HeapTable(Pager& pager) : pager(pager) {}


RID HeapTable::insert(const void* data, uint16_t size) {

    // Try existing pages
    for (uint32_t p_id = 0; p_id < pager.get_total_pages(); ++p_id) {

        Page page;
        pager.read_page(p_id, page);   

        if (page.can_fit(size)) {
            int s_id = page.insert_tuple(data, size);
            if (s_id < 0) {
                throw std::runtime_error("Insert failed");
            }

            pager.write_page(p_id, page);  
            return RID{p_id, static_cast<uint16_t>(s_id)};
        }
    }

    // No page fits → allocate new page
    uint32_t new_p_id = pager.allocate_page();

    Page page;
    pager.read_page(new_p_id, page);   

    int s_id = page.insert_tuple(data, size);
    if (s_id < 0) {
        throw std::runtime_error("Insert failed on new page");
    }

    pager.write_page(new_p_id, page);  
    return RID{new_p_id, static_cast<uint16_t>(s_id)};
}

bool HeapTable::delete_tuple(const RID& rid){
    Page page;
    
    //  Page object in RAM RID = (0, 2) page = 0 and slot = 2
    if(!pager.read_page(rid.page_id , page)){
        return false;
    }
    
    // Delete the slot inside the page -> Slot[2] → marked as deleted
    if(!page.delete_tuple(rid.slot_id)){
        return false;
    }

    //write page 0 back to disk 
    pager.write_page(rid.page_id,page);

    return true;
}



std::vector<std::vector<char>> HeapTable::scan_all() {
    std::vector<std::vector<char>> res;

    for (uint32_t p_id = 0; p_id < pager.get_total_pages(); ++p_id) {

        Page page;
        pager.read_page(p_id, page);   

        uint16_t num_slots = page.get_num_slots();

        for (uint16_t slot = 0; slot < num_slots; ++slot) {
            auto tuple = page.read_tuple(slot);
            if (!tuple.empty()) {
                res.push_back(std::move(tuple));
            }
        }
    }

    return res;
}

std::vector<RowRef> HeapTable::scan_with_rid() {
    std::vector<RowRef> result;

    for (uint32_t p_id = 0; p_id < pager.get_total_pages(); ++p_id) {
        Page page;

        if (!pager.read_page(p_id, page))
            continue;

        uint16_t num_slots = page.get_num_slots();

        for (uint16_t s_id = 0; s_id < num_slots; ++s_id) {
            auto tuple = page.read_tuple(s_id);

            if (!tuple.empty()) {
                RowRef ref;
                ref.rid = RID{p_id, s_id};
                ref.data = std::move(tuple);

                result.push_back(std::move(ref));
            }
        }
    }

    return result;
}

