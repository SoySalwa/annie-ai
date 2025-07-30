#ifndef FILE_SYSTEM_HPP
#define FILE_SYSTEM_HPP

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

class FileSystem
{
private:
    static inline const std::vector<std::string> files_types = {
        ".cmake",
        ".txt",
        ".json",
        ".cpp",
        ".c",
        ".cc",
        ".h",
        ".hpp"};

public:
    FileSystem() = default;
    ~FileSystem() = default;

    static std::string read_files()
    {
        std::string path = std::filesystem::current_path().string();
        std::stringstream results;

        for (const auto &entry : std::filesystem::recursive_directory_iterator(path))
        {
            if (entry.is_regular_file() &&
                std::find(files_types.begin(), files_types.end(), entry.path().extension()) != files_types.end())
            {
                std::ifstream file(entry.path());
                if (file.is_open())
                {
                    results << "// File: " << entry.path().string() << "\n";
                    results << file.rdbuf() << "\n\n";
                }
            }
        }

        return results.str();
    }
};

#endif // FILE_SYSTEM_HPP