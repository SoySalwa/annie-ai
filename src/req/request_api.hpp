#ifndef REQUEST_API_HPP
#define REQUEST_API_HPP

#include <iostream>
#include <vector>
#include <string>
#include <curl/curl.h>

struct DataRequest
{
    std::string url;
    std::vector<std::string> headers;
    std::string contents; // JSON u otro cuerpo de la solicitud

    DataRequest(const std::string &url,
                const std::vector<std::string> &headers = {},
                const std::string &contents = "")
        : url(url), headers(headers), contents(contents) {}
};

// Callback para guardar la respuesta
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t totalSize = size * nmemb;
    std::string *response = static_cast<std::string *>(userp);
    response->append(static_cast<char *>(contents), totalSize);
    return totalSize;
}

struct RequestAPI
{
    std::string send_request(const DataRequest &request)
    {
        CURL *curl;
        CURLcode res;
        std::string response;

        curl = curl_easy_init();
        if (curl)
        {
            // URL
            curl_easy_setopt(curl, CURLOPT_URL, request.url.c_str());

            // Headers
            struct curl_slist *chunk = nullptr;
            for (const auto &header : request.headers)
            {
                chunk = curl_slist_append(chunk, header.c_str());
            }
            if (chunk)
            {
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            }

            // Método POST
            curl_easy_setopt(curl, CURLOPT_POST, 1L);

            // Contenido JSON u otro
            if (!request.contents.empty())
            {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.contents.c_str());
            }

            // Callback para capturar la respuesta
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            // Ejecutar
            res = curl_easy_perform(curl);
            if (res != CURLE_OK)
            {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            }

            // Limpiar
            if (chunk)
                curl_slist_free_all(chunk);
            curl_easy_cleanup(curl);
        }

        return response;
    }
};

#endif // REQUEST_API_HPP
