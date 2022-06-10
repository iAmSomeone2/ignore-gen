#include "IgnoreEntry.hpp"
#include <sstream>

IgnoreEntry::IgnoreEntry(const std::string& key, const std::string& name, const std::string& contents) : 
    key(key), name(name), contents(contents) {}

const std::string& IgnoreEntry::getKey() const {
    return this->key;
}

const std::string& IgnoreEntry::getName() const {
    return this->name;
}

const std::string& IgnoreEntry::getContents() const {
    return this->contents;
}

const std::string IgnoreEntry::toString() const {
    std::stringstream str;
    str << "{\n  \"key\": \"" << this->key << "\",";
    str << "\n  \"name\": \"" << this->name << "\",";
    str << "\n  \"contents\": \"" << this->contents << "\"\n}";

    return str.str();
}
