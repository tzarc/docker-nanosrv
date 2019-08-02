#include <algorithm>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <cstdlib>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace fs = std::filesystem; // from <filesystem>
using path = fs::path; // from <filesystem>
namespace beast = boost::beast;   // from <boost/beast.hpp>
namespace http = beast::http;     // from <boost/beast/http.hpp>
namespace net = boost::asio;      // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

#include "listener.hpp"

// Relinquish root permissions if specified through $PUID and $PGID
void drop_root_permissions()
{
    const char* puid = std::getenv("PUID");
    const char* pgid = std::getenv("PGID");

    if(pgid)
    {
        int new_gid = std::stoi(std::string(pgid));
        if(setgid(new_gid) != 0)
            throw new std::runtime_error("Failed to set gid");
    }

    if(puid)
    {
        int new_uid = std::stoi(std::string(puid));
        if(setuid(new_uid) != 0)
            throw new std::runtime_error("Failed to set uid");
    }
}

int main(int argc, char* argv[])
{
    try
    {
        drop_root_permissions();

        const char* bind_address = std::getenv("BIND_ADDRESS");
        if(!bind_address)
            bind_address = "0.0.0.0";

        const char* bind_port_str = std::getenv("BIND_PORT");
        if(!bind_port_str)
            bind_port_str = "25152";

        const char* doc_root_str = std::getenv("DOC_ROOT");
        if(!doc_root_str)
            doc_root_str = "/www";

		path doc_root = doc_root_str;
        if(!fs::is_directory(doc_root) || fs::absolute(doc_root) == "/")
        throw std::runtime_error("Can't export '" + std::string(doc_root_str) + "' (resolved to '" + fs::absolute(doc_root).native() + "')");

        const char* num_threads_str = std::getenv("NUM_THREADS");
        if(!num_threads_str)
            num_threads_str = "1";

        auto const address = net::ip::make_address(bind_address);
        unsigned short port = static_cast<unsigned short>(std::stoi(std::string(bind_port_str)));
        int num_threads = std::stoi(num_threads_str);

        std::cout << "Binding to " << bind_address << ":" << port << std::endl;
        std::cout << "Exporting from " << doc_root << std::endl;

        net::io_context ioc{1};

        std::make_shared<listener>(ioc, tcp::endpoint{address, port}, std::make_shared<std::string>(std::move(doc_root)))->run();

        std::vector<std::thread> threads;
        threads.reserve(num_threads - 1);
        for(int i = 0; i < num_threads - 1; ++i)
            threads.emplace_back([&ioc] { ioc.run(); });
        ioc.run();

        for(auto&& t : threads)
            t.join();
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}