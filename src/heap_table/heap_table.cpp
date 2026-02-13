#include "heap_table.h"
#include <iostream>

HeapTable::HeapTable(BufferPool& buffer) : buffer(buffer) {}



RID HeapTable::insert(const void* data, uint16_t size) {

    // Try existing pages
    for (uint32_t p_id = 0; p_id < buffer.get_pager().get_total_pages() ; ++p_id) {

        Page& page = buffer.fetch_page(p_id);


        if (page.can_fit(size)) {
            int s_id = page.insert_tuple(data, size);

            if (s_id < 0) {
                throw std::runtime_error("Insert failed");
            }

            buffer.mark_dirty(p_id);  
            return RID{p_id, static_cast<uint16_t>(s_id)};
        }
    }

    // No page fits → allocate new page
    uint32_t new_p_id = buffer.allocate_page();
    Page& page = buffer.fetch_page(new_p_id); 

    int s_id = page.insert_tuple(data, size);

    if (s_id < 0) {
        throw std::runtime_error("Insert failed on new page");
    }

    return RID{new_p_id, static_cast<uint16_t>(s_id)} ;
}

bool HeapTable::delete_tuple(const RID& rid){

    Page& page = buffer.fetch_page(rid.page_id);

    bool ok = page.delete_tuple(rid.slot_id);

    if (ok) {
        buffer.mark_dirty(rid.page_id);
    }

    return ok;
}



std::vector<std::vector<char>> HeapTable::scan_all() {
    std::vector<std::vector<char>> res;

    for (uint32_t p_id = 0; p_id < buffer.get_pager().get_total_pages(); ++p_id) {

        Page& page = buffer.fetch_page(p_id);  

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

    for (uint32_t p_id = 0; p_id < buffer.get_pager().get_total_pages(); ++p_id) {
        Page& page = buffer.fetch_page(p_id);

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

