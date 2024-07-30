#pragma once

#include "net_headers.hpp"

namespace webino::net
{
    enum class AddressFamily: int
    {
        IPV4 = AF_INET,
        IPV6 = AF_INET6,
        UNSPEC = AF_UNSPEC, // USED FOR LOOKUP HINTS
    };

    enum class SocketType: int
    {
        STREAM = SOCK_STREAM,
        DGRAM = SOCK_DGRAM,
        ANY = 69420, // USED FOR LOOKUP HINTS
    };

    enum class SocketProtocol: int
    {
        TCP = IPPROTO_TCP,
        UDP = IPPROTO_UDP,
        ANY = 69420, // USED FOR LOOKUP HINTS
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