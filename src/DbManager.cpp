#include "DbManager.hpp"
#include <stdexcept>
#include <fmt/format.h>

DbManager::DbManager(const std::string& dbPath, int flags) {
    if (sqlite3_open_v2(dbPath.c_str(), &(this->dbConnection), flags, nullptr) != SQLITE_OK) {
        throw std::runtime_error(fmt::format("[ERROR] %s", this->getSqliteError()));
    }
}

DbManager::~DbManager() {
    sqlite3_close_v2(this->dbConnection);
}

const std::string DbManager::getSqliteError() const {
    const std::string errCodeStr = sqlite3_errstr(sqlite3_errcode(this->dbConnection));
    const std::string errMsg = sqlite3_errmsg(this->dbConnection);

    return fmt::format("%s: %s", errCodeStr, errMsg);
}

std::shared_ptr<sqlite3_stmt> DbManager::prepareStatement(const std::string& sqlStr){
    sqlite3_stmt* stmt = nullptr;
    const int status = sqlite3_prepare_v2(this->dbConnection, sqlStr.c_str(), sqlStr.length(), &stmt, nullptr);
    auto stmtPtr = std::shared_ptr<sqlite3_stmt>(stmt, sqlite3_finalize);
    if (status != SQLITE_OK) {
        throw std::runtime_error(fmt::format("[ERROR]: %s", this->getSqliteError()));
    }

    return stmtPtr;
}

uint_fast32_t DbManager::getEntryCount(){
    auto stmt = this->prepareStatement("select count(\"key\") from \"gitignore\";");

    const int status = sqlite3_step(stmt.get());
    if (status == SQLITE_ERROR || status == SQLITE_MISUSE) {
        throw std::runtime_error(fmt::format("[ERROR]: %s", this->getSqliteError()));
    }
    const uint_fast32_t result = sqlite3_column_int(stmt.get(), 0);

    return result;
}

std::vector<std::string> DbManager::getAllKeys() {
    auto stmt = this->prepareStatement("select \"key\" from \"gitignore\" order by \"key\" asc;");

    std::vector<std::string> keys;
    auto status = SQLITE_OK;
    while (status != SQLITE_DONE) {
        status = sqlite3_step(stmt.get());
        if (status == SQLITE_ERROR || status == SQLITE_MISUSE) {
            throw std::runtime_error(fmt::format("[ERROR]: %s", this->getSqliteError()));
        } else if (status == SQLITE_DONE) {
            break;
        }

        keys.emplace_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 0)));
    }

    return keys;
}

IgnoreEntry DbManager::getEntryForKey(const std::string& key) {
    auto stmt = this->prepareStatement("select \"name\",\"contents\" from \"gitignore\" where \"key\" = ?;");
    if (sqlite3_bind_text(stmt.get(), 1, key.c_str(), key.length(), nullptr) != SQLITE_OK) {
        throw std::runtime_error(fmt::format("[ERROR]: %s", this->getSqliteError()));
    }
    
    std::string errMsg = "";
    switch (sqlite3_step(stmt.get())) {
        case SQLITE_ROW:
            return IgnoreEntry(key,
                reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 0)),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 1)));
        case SQLITE_DONE:
            errMsg = fmt::format("[WARN] No entry found for key: %s", key);
            throw std::runtime_error(errMsg);
        default:
            errMsg = fmt::format("[ERROR]: %s", this->getSqliteError());
            throw std::runtime_error(errMsg);
    }

    
}
