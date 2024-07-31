#pragma once

#include "net_headers.hpp"

namespace webino::net
{
    enum class AddressFamily: int
    {
        IPV4 = AF_INET,
        IPV6 = AF_INET6,
        UNSPEC = AF_UNSPEC, 
        ANY = 0, // AF_UNSPEC 
    };

    enum class SocketType: int
    {
        STREAM = SOCK_STREAM,
        DGRAM = SOCK_DGRAM,
        ANY = 0,
    };

    enum class SocketProtocol: int
    {
        TCP = IPPROTO_TCP,
        UDP = IPPROTO_UDP,
        ANY = 0, 
    };

    enum class OptionLevel: int
    {
        SOCKET = SOL_SOCKET,
    };

    enum class Option: int
    {
        SOCKET_TYPE = SO_TYPE,
    };
}