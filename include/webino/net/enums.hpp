#pragma once

#include "net_necessities.hpp"

namespace webino::net
{
    enum class SocketAddressFamily
    {
        IPV4 = AF_INET,
        IPV6 = AF_INET6,
        UNSPEC = AF_UNSPEC, // for lookup but we don't handle that shii xd
    };

    enum class SocketType
    {
        STREAM = SOCK_STREAM,
        DGRAM = SOCK_DGRAM,
    };

    enum class SocketProtocol
    {
        TCP = IPPROTO_TCP,
        UDP = IPPROTO_UDP,
        ANY = 0,
    };

    enum class SocketMode
    {
        PLAIN = 0,
        TLS = 1,
    };
}