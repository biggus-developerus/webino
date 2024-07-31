#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "webino/errors.hpp"
#include "enums.hpp"

namespace webino::net
{
    struct LookupRes
    {
        std::string addr;
        
        AddressFamily addr_family;
        SocketType sock_type;
        SocketProtocol sock_proto;
    };

    template<
        AddressFamily addr_family = AddressFamily::UNSPEC, 
        SocketType sock_type = SocketType::ANY,
        SocketProtocol sock_proto = SocketProtocol::ANY    
    >
    std::vector<LookupRes> lookup(std::string_view addr)
    {
        struct addrinfo hints = {0};
        struct addrinfo* res;

        hints.ai_family = (int)addr_family;
        if constexpr(sock_type != SocketType::ANY)
            hints.ai_socktype = (int)sock_type;
        if constexpr(sock_proto != SocketProtocol::ANY)
            hints.ai_protocol = (int)sock_proto;

        std::string copy = std::string(addr);
        if (getaddrinfo(copy.data(), NULL, &hints, &res) != 0)
        {
            if constexpr (addr_family != AddressFamily::UNSPEC)
                throw webino::errors::SockError("getaddrinfo failed (POSSIBLE: No address found for the specified family): " + webino::errors::get_err_str(webino::errors::get_errno(true))); 
            else
                throw webino::errors::SockError("getaddrinfo failed: " + webino::errors::get_err_str(webino::errors::get_errno(true)));
        }

        std::vector<LookupRes> addrs;

        struct addrinfo* n = res;
        int buff_size = INET6_ADDRSTRLEN;
        
        if constexpr (addr_family == AddressFamily::IPV4)
            buff_size = INET_ADDRSTRLEN;
        else if constexpr (addr_family == AddressFamily::IPV6)
            buff_size = INET6_ADDRSTRLEN;

        char buff[buff_size];

        while (n != NULL)
        {
            memset(buff, 0, buff_size);
            
            void* addr = nullptr;
            if constexpr (addr_family == AddressFamily::UNSPEC)
            {
                if (n->ai_family == AF_INET)
                    addr = &((struct sockaddr_in*)n->ai_addr)->sin_addr;
                else if (n->ai_family == AF_INET6)
                    addr = &((struct sockaddr_in6*)n->ai_addr)->sin6_addr;
                else 
                    throw webino::errors::WebinoError("Got unknown address family for ai_family");
            }
            else if constexpr (addr_family == AddressFamily::IPV4)
                addr = &((struct sockaddr_in*)n->ai_addr)->sin_addr;
            else if constexpr (addr_family == AddressFamily::IPV6)
                addr = &((struct sockaddr_in6*)n->ai_addr)->sin6_addr;

            if (inet_ntop(n->ai_family, addr, buff, buff_size) == NULL)
                throw webino::errors::SockError("inet_ntop failed: " + webino::errors::get_err_str(webino::errors::get_errno(true)));

            addrs.push_back({buff, (AddressFamily)n->ai_family, (SocketType)n->ai_socktype, (SocketProtocol)n->ai_protocol});
            n = n->ai_next;
        }

        freeaddrinfo(res); // it'll also free the nodes so all goochie on skib
            
        return addrs;
    }
}