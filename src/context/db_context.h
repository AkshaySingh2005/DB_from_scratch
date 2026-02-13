#pragma once
#include <string>
#include <memory>
#include "buffer_pool/buffer_pool.h"
#include "page/pager.h"

struct DatabaseContext {
    std::string current_database;

    std::unique_ptr<Pager> pager;
    std::unique_ptr<BufferPool> buffer;
};

extern DatabaseContext db_ctx;
