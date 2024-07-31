#include "webino/net/net.hpp"
#include "webino/net/net_headers.hpp"

#ifdef _WIN32
    #include "webino/errors.hpp"

    #define WINSOCK_VERSION_MAJOR 2
    #define WINSOCK_VERSION_MINOR 2
#endif

#include <iostream>
#include <fmt/core.h>

namespace webino::net
{
    void initialise()
    {
        #ifdef _WIN32
            WSADATA data;
            int result = WSAStartup(MAKEWORD(WINSOCK_VERSION_MAJOR, WINSOCK_VERSION_MINOR), &data);
            if (result != 0)
                throw webino::errors::WinSockError(webino::errors::get_err_str(result));
            
            if (LOBYTE(data.wVersion) != WINSOCK_VERSION_MAJOR || HIBYTE(data.wVersion) != WINSOCK_VERSION_MINOR)
            {
                WSACleanup();
                throw webino::errors::WinSockError(
                    fmt::format(
                        "Couldn't find a usable winsock DLL. Expected version: {}.{}, found {}.{}",
                        WINSOCK_VERSION_MAJOR, WINSOCK_VERSION_MINOR,
                        (int)LOBYTE(data.wVersion), (int)HIBYTE(data.wVersion)
                    )
                );
            }
        #endif

        _initialised = true;
    }

    void deinitialise()
    {
        #ifdef _WIN32
            WSACleanup();
        #endif

        _initialised = false;
    }    

    std::optional<SocketVariant> _connect(std::string_view domain, int port) // port from the scheme.. on skib
    {
        std::optional<net::SocketVariant> sock = std::nullopt;
        net::LookupRes info;

        for (auto res: net::lookup<net::AddressFamily::UNSPEC, net::SocketType::STREAM, net::SocketProtocol::TCP>(domain)) // I mean on skib my skibski, this is a HTTP client so laikee...??
        {
            if (!(sock = net::_get_appropriate_sock(res)))
                continue;
                
            info = res;
        }
        
        if (!sock.has_value())
            throw errors::WebinoError(fmt::format("Failed to connect to {}, couldn't create a compatable socket.", info.addr));

        std::visit([&](auto& sock) 
            {
                try { sock.connect(info.addr, port); } 
                catch (const errors::WebinoError& e) { std::cerr << "Connection error: " << e.what() << std::endl; }
            }, 
            sock.value()
        );

        return sock;
    }
}