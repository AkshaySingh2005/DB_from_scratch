#pragma once

#include "page/pager.h"
#include "page/page.h"
#include "buffer_pool/buffer_pool.h"
#include "rid.h"
#include <vector>

struct RowRef {
    RID rid;
    std::vector<char> data;
};

class HeapTable {
private:
    BufferPool& buffer;

public:
    HeapTable(BufferPool& buffer);

    RID insert(const void* data, uint16_t size);
    bool delete_tuple(const RID& rid);
    std::vector<std::vector<char>> scan_all();

    std::vector<RowRef> scan_with_rid();   
};


