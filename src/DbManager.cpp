#include "DbManager.hpp"
#include <stdexcept>
#include <sstream>

DbManager::DbManager(const std::string& dbPath, int flags) {
    std::stringstream errMsg;
    if (sqlite3_open_v2(dbPath.c_str(), &(this->dbConnection), flags, nullptr) != SQLITE_OK) {
        errMsg << "[ERROR] " << this->getSqliteError();
        throw std::runtime_error(errMsg.str());
    }
}

DbManager::~DbManager() {
    sqlite3_close_v2(this->dbConnection);
}

std::string DbManager::getSqliteError() const {
    std::stringstream errMsg;

    errMsg << "{sqlite error}: " << sqlite3_errmsg(this->dbConnection);
    return errMsg.str();
}

std::shared_ptr<sqlite3_stmt> DbManager::prepareStatement(const std::string& sqlStr){
    std::stringstream errMsg;
    sqlite3_stmt* stmt = nullptr;
    const int status = sqlite3_prepare_v2(this->dbConnection, sqlStr.c_str(), sqlStr.length(), &stmt, nullptr);
    auto stmtPtr = std::shared_ptr<sqlite3_stmt>(stmt, sqlite3_finalize);
    if (status != SQLITE_OK) {
        errMsg << "[ERROR] " << this->getSqliteError();
        throw std::runtime_error(errMsg.str());
    }

    return stmtPtr;
}

uint_fast32_t DbManager::getEntryCount(){
    auto stmt = this->prepareStatement("select count(\"key\") from \"gitignore\";");

    std::stringstream errMsg;
    const int status = sqlite3_step(stmt.get());
    if (status == SQLITE_ERROR || status == SQLITE_MISUSE) {
        errMsg << "[ERROR] " << this->getSqliteError();
        throw std::runtime_error(errMsg.str());
    }
    const uint_fast32_t result = sqlite3_column_int(stmt.get(), 0);

    return result;
}

std::vector<std::string> DbManager::getAllKeys() {
    auto stmt = this->prepareStatement("select \"key\" from \"gitignore\" order by \"key\" asc;");

    std::stringstream errMsg;
    std::vector<std::string> keys;
    auto status = SQLITE_OK;
    while (status != SQLITE_DONE) {
        status = sqlite3_step(stmt.get());
        if (status == SQLITE_ERROR || status == SQLITE_MISUSE) {
            errMsg << "[ERROR] " << this->getSqliteError();
            throw std::runtime_error(errMsg.str());
        } else if (status == SQLITE_DONE) {
            break;
        }

        keys.emplace_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 0)));
    }

    return keys;
}

IgnoreEntry DbManager::getEntryForKey(const std::string& key) {
    std::stringstream errMsg;
    auto stmt = this->prepareStatement("select \"name\",\"contents\" from \"gitignore\" where \"key\" = ?;");
    if (sqlite3_bind_text(stmt.get(), 1, key.c_str(), key.length(), nullptr) != SQLITE_OK) {
        errMsg << "[ERROR] " << this->getSqliteError();
        throw std::runtime_error(errMsg.str());
    }
    
    switch (sqlite3_step(stmt.get())) {
        case SQLITE_ROW:
            return IgnoreEntry(key,
                reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 0)),
                reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 1)));
        case SQLITE_DONE:
            errMsg << "[WARN] No entry found for key: " << key;
            throw std::runtime_error(errMsg.str());
        default:
            errMsg << "[ERROR] " << this->getSqliteError();
            throw std::runtime_error(errMsg.str());
    }
}
