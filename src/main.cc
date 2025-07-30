#include "req/request_api.hpp"

#include <nlohmann/json.hpp>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include "data/file_system.hpp"
#include "data/history.hpp"
#include <thread>
#include <chrono>
#include <atomic>

void showLoadingCircle(std::atomic<bool> &running)
{
    const std::vector<std::string> spinner = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
    ;
    size_t spinner_index = 0;

    while (running.load())
    {
        std::cout << "\r" << spinner[spinner_index++ % spinner.size()] << " Thinking..." << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "\r" << std::string(20, ' ') << "\r";
}

std::vector<nlohmann::json> chat_history;

int main()
{
    std::cout << "\033[32m" <<
        R"(
__        __   _                            _ _ 
\ \      / /__| | ___ ___  _ __ ___   ___  | | |
 \ \ /\ / / _ \ |/ __/ _ \| '_ ` _ \ / _ \ | | |
  \ V  V /  __/ | (_| (_) | | | | | |  __/ |_|_|
   \_/\_/ \___|_|\___\___/|_| |_| |_|\___| (_|_)
                                                          
        Welcome to AnnieAI Chatbot Console
)" << "\033[0m"
              << "\n";

    std::cout << "\033[93mType 'exit' or 'quit' to exit.\033[0m" << std::endl;
    std::cout << "\033[93mType 'clear_history' to clear the chat history.\033[0m" << std::endl;
    std::cout << "\n";

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

        DataRequest request(url, headers, body);
        RequestAPI api;
        std::atomic<bool> loading(true);
        std::thread loading_thread(showLoadingCircle, std::ref(loading));

        std::string response = api.send_request(request);

        loading = false;
        loading_thread.join();
        std::cout << "\033[32m[You]:\033[0m " << text << std::endl;
        try
        {
            nlohmann::json data = nlohmann::json::parse(response);
            std::string ai_response = data["candidates"][0]["content"]["parts"][0]["text"];
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
