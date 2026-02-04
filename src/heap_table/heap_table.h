#pragma once

#include "page/pager.h"
#include "page/page.h"
#include "rid.h"
#include <vector>

class HeapTable {
    private:
       Pager& pager;
    public:
       HeapTable(Pager& pager);
       
       RID insert(const void* data , uint16_t size);

       std::vector<std::vector<char>> scan_all();
};

