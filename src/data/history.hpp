#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

void save_history(const std::vector<nlohmann::json> &history, const std::string &filename = "src/data/chat_history.json")
{
    std::filesystem::create_directories("src/data");
    std::ofstream out(filename);
    if (out.is_open())
    {
        nlohmann::json j;
        j["contents"] = history;
        out << j.dump(2);
    }
}

void load_history(std::vector<nlohmann::json> &history, const std::string &filename = "src/data/chat_history.json")
{
    std::ifstream in(filename);
    if (in.is_open())
    {
        nlohmann::json j;
        in >> j;
        if (j.contains("contents") && j["contents"].is_array())
        {
            history = j["contents"].get<std::vector<nlohmann::json>>();
        }
    }
}