#include <projconfig.h>
#include "DbManager.hpp"
#include "IgnoreEntry.hpp"

#include <fmt/printf.h>

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <array>

#include <string.h>
#include <getopt.h>

namespace fs = std::filesystem;

void printUsage() {
    std::cerr << "usage: " << PROGRAM_NAME << " [OPTION]\n";
    std::cerr << "  -h, --help\t\t\tPrint this message and exit.\n";
    std::cerr << "  -l, --list-keys\t\tList all .gitignore keys and exit.\n";
}

void writeGitignore(const std::vector<IgnoreEntry>& entries, const fs::path& outPath) {
    auto outFile = std::ofstream(outPath);
    if (!outFile.is_open()) {
        throw std::runtime_error("[ERROR]: Failed to open output file.");
    }

    for (const auto& entry : entries) {
        outFile << entry.getContents();
    }

    outFile.close();
}

int main(int argc, char **argv) {
    // Set up arguments
    int helpFlag = 0;
    int listKeysFlag = 0;
    bool outputSpecified = false;

    struct option longopts[] = {
        {"help", no_argument, &helpFlag, 1},
        {"list-keys", optional_argument, &listKeysFlag, 'l'},
        {"output", required_argument, nullptr, 'o'}
    };

    int opt;
    // std::string outPath;
    fs::path outPath;
    while (true) {
        opt = getopt_long(argc, argv, "hlo:", longopts, 0);
        // return if out of options
        if (opt == -1) break;

        switch (opt) {
            case 'h':
                helpFlag = 1;
                break;
            case 'l':
                listKeysFlag = 1;
                break;
            case 'o':
                outPath = fs::path(optarg);
                outputSpecified = true;
                break;
            case '?':
                if (optopt == 'o') {
                    fmt::print(stderr, "Option -%c requires an argument.\n", optopt);
                } else if (isprint(optopt)) {
                    fmt::print(stderr, "Unknown option \'-%c\'.\n", optopt);
                }
                return 1;
            default:
                break;
        }
    }

    if (helpFlag) {
        printUsage();
        return 0;
    }

    if (!outputSpecified) {
        // TODO: setup fallback to stdout.
        std::cerr << "[ERROR] Output file must be specified.\n";
        return 1;
    }

    auto db = DbManager(DB_FILE, SQLITE_OPEN_READONLY);
    
    if (listKeysFlag) {
        auto keys = db.getAllKeys();
        for (const auto& key : keys) {
            std::cout << key << '\n';
        }
        return 0;
    }

    std::vector<std::string> searchKeys;
    auto buf = std::array<char, 32>();
    for (int i = optind; i < argc; i++) {
        strncpy(buf.data(), argv[i], 32);
        searchKeys.emplace_back(buf.data());
    }

    // Construct .gitignore using the provided keys
    std::vector<IgnoreEntry> entries;
    for (const auto& key : searchKeys) {
        try {
            auto entry = db.getEntryForKey(key);
            entries.push_back(entry);
        } catch (const std::runtime_error& ex) {
            std::cerr << ex.what() << '\n';
        }
    }

    try {
        writeGitignore(entries, outPath);
    } catch (const std::runtime_error& ex) {
        std::cerr << ex.what() << '\n';
        return 2;
    }

    return 0;
}
