#pragma once

#include "IgnoreEntry.hpp"

#include <sqlite3.h>
#include <string>
#include <cstdint>
#include <memory>
#include <vector>

class DbManager {
public:
    DbManager(const std::string& dbPath, int flags);

    ~DbManager();

    uint_fast32_t getEntryCount();

    std::vector<std::string> getAllKeys();

    IgnoreEntry getEntryForKey(const std::string& key);
private:
    sqlite3* dbConnection = nullptr;

    std::string getSqliteError() const;

    std::shared_ptr<sqlite3_stmt> prepareStatement(const std::string& sqlStr);
};
