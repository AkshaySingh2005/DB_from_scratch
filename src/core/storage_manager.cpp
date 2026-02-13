#include "storage_manager.h"
#include <filesystem>

namespace fs = std::filesystem;

void open_table_storage(const std::string& table_name) {

    std::string db_file =
        "../data/" + db_ctx.current_database +
        "/" + table_name +
        "/" + table_name + ".db";

    // Create pager
    db_ctx.pager = std::make_unique<Pager>(db_file);

    // Create buffer pool (size = 10 pages)
    db_ctx.buffer = std::make_unique<BufferPool>(*db_ctx.pager, 10);
}
