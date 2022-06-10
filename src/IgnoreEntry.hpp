#pragma once

#include <string>

class IgnoreEntry {
public:
    IgnoreEntry(const std::string& key, const std::string& name, const std::string& contents);

    const std::string& getKey() const;
    const std::string& getName() const;
    const std::string& getContents() const;

    const std::string toString() const;
private:
    std::string key;
    std::string name;
    std::string contents;
};
