#include <projconfig.h>
#include "DbManager.hpp"
#include "IgnoreEntry.hpp"

#include <fmt/printf.h>

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <set>
#include <array>

#include <string.h>
#include <getopt.h>

namespace fs = std::filesystem;

void printUsage() {
    std::cerr << "\n\t" << PROGRAM_NAME << " " << PROGRAM_VERSION << "\n\n"; 
    std::cerr << "usage: " << PROGRAM_NAME << " [OPTION] <gitignore-keys>\n";
    std::cerr << "  -h, --help\t\t\t\t\tPrint this message and exit.\n";
    std::cerr << "  -o, --output=</path/ignorefile>\t\tCustom path to write .gitignore file to.\n";
    std::cerr << "  -l, --list-keys\t\t\t\tList all .gitignore keys and exit.\n";
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

    struct option longopts[] = {
        {"help", no_argument, &helpFlag, 1},
        {"list-keys", no_argument, &listKeysFlag, 'l'},
        {"output", required_argument, nullptr, 'o'}
    };

    int opt;
    // std::string outPath;
    fs::path outPath = fs::path("./.gitignore");
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
                break;
            case '?':
                // if (optopt == 'o') {
                //     std::cerr << "Option \'-" << static_cast<char>(optopt) << "\' requires an argument.\n";
                //     // fmt::print(stderr, "Option -%c requires an argument.\n", optopt);
                // } else if (isprint(optopt)) {
                //     std::cerr << "Unknown option \'-" << static_cast<char>(optopt) << "\'.\n";
                //     // fmt::print(stderr, "Unknown option \'-%c\'.\n", optopt);
                // }
                return 1;
            default:
                break;
        }
    }

    if (helpFlag) {
        printUsage();
        return 0;
    }

    auto db = DbManager(DB_FILE, SQLITE_OPEN_READONLY);
    
    if (listKeysFlag) {
        auto keys = db.getAllKeys();
        for (const auto& key : keys) {
            std::cout << key << '\n';
        }
        return 0;
    }

    std::set<std::string> searchKeys;
    auto buf = std::array<char, 33>();
    for (int i = optind; i < argc; i++) {
        strncpy(buf.data(), argv[i], 32);
        searchKeys.emplace(buf.data());
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
