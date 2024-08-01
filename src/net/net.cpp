#include "webino/net/net.hpp"
#include "webino/net/net_headers.hpp"

#ifdef _WIN32
    #include "webino/errors.hpp"

    #define WINSOCK_VERSION_MAJOR 2
    #define WINSOCK_VERSION_MINOR 2
#endif

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
}