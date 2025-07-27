#include "req/request_api.hpp"

#include <nlohmann/json.hpp>
#include <cstdlib>
#include <fstream>
#include <filesystem>

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
std::vector<nlohmann::json> chat_history;

int main()
{
    // dotenv::init();
    // const char *api_key_cstr = std::getenv("GOOGLE_API_TOKEN");

    /*  if (!api_key_cstr)
     {
         std::cerr << "Error: GOOGLE_API_TOKEN environment variable is not set." << std::endl;
         return 1;
     } */

    /*     std::string api_key = api_key_cstr;
     */
    std::string url = "https://backend-annie-ai.vercel.app/api/chat";

    std::vector<std::string>
        headers = {
            "Content-Type: application/json"};
    load_history(chat_history);

    bool is_inserted = chat_history.empty();

    std::string prompt_text;

    while (true)
    {
        std::cout << "Your message: ";
        std::string text;
        std::getline(std::cin, text);
        if (text == "clear_history")
        {
            chat_history.clear();
            std::filesystem::remove("src/data/chat_history.json");
            std::cout << "Chat history cleared." << std::endl;
            is_inserted = true;
            continue;
        }

        if (text.empty() || text == "exit" || text == "quit")
        {
            std::cout << "Exiting chat." << std::endl;
            break;
        }
        chat_history.push_back({{"role", "user"},
                                {"parts", {{{"text", text}}}}});

        std::vector<nlohmann::json> current_request = chat_history;

        nlohmann::json chat_history_json;
        chat_history_json["contents"] = current_request;
        std::string body = chat_history_json.dump();

        /*
                std::string body = R"({
            "contents": [
                {
                    "parts": [
                        {
                            "text": ")" +
                                   text + R"("
                        }
                    ]
                }
            ]
        })"; */

        DataRequest request(url, headers, body);
        RequestAPI api;
        std::string response = api.send_request(request);
        std::cout << "\033[32m[You]:\033[0m " << text << std::endl;
        try
        {
            nlohmann::json data = nlohmann::json::parse(response);
            std::string ai_response = data["candidates"][0]["content"]["parts"][0]["text"];
            std::cout << "Thinking...\n";
            std::cout << "\033[34m[AnnieAI]:\033[0m " << ai_response << std::endl;
            chat_history.push_back({{"role", "model"},
                                    {"parts", {{{"text", ai_response}}}}});
        }
        catch (const nlohmann::json::parse_error &e)
        {
            std::cerr << "Error parsing JSON response: " << e.what() << std::endl;
            return 1;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error accesing JSON fields: " << e.what() << std::endl;
            return 1;
        }
        save_history(chat_history);
    }
    return 0;
}
