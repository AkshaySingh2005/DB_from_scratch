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
