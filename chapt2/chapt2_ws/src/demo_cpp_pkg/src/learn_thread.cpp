#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <functional>
#include "cpp-httplib/httplib.h"

class Download 
{
public:
    void download(const std::string& url, const std::string& path, 
    std::function<void(const std::string&, const std::string&)> word_count_callback)
    {
        httplib::Client cli(url);
        auto res = cli.Get(path);

        if (res && res->status == 200) 
        {
            // Call the callback function with the result
            word_count_callback(path, res->body);
        } 
        else 
        {
            std::cerr << "Failed to download from " << url + path << std::endl;
        }
    }

    void start_download(const std::string& url, const std::string& path,
    std::function<void(const std::string&, const std::string&)> word_count_callback)
    {
        std::thread download_thread(&Download::download, this, url, path, word_count_callback);
        download_thread.detach();
    }
};

int main() 
{
    Download downloader;

    // Define the callback function to count words
    auto word_count_callback = [](const std::string& path, const std::string& content) 
    {
        std::cout << "Downloaded from " << path << " with content: " << content << std::endl;
    };

    // Start the download in a separate thread
    downloader.start_download("http://0.0.0.0:8000", "/novel1.txt", word_count_callback);
    downloader.start_download("http://0.0.0.0:8000", "/novel2.txt", word_count_callback);
    downloader.start_download("http://0.0.0.0:8000", "/novel3.txt", word_count_callback);

    // Keep the main thread alive for demonstration purposes
    std::this_thread::sleep_for(std::chrono::seconds(5));

    return 0;
}