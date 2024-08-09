#pragma once

#include <vector>
#include <string_view>

#include "enums.hpp"
#include "net_necessities.hpp"
#include "socket.hpp"

#include "webino/result.hpp"
#include "webino/logging.hpp"

namespace webino::net
{
    // why are we using templates..?? idk. they just look cool xd.
    template<SocketAddressFamily sock_addr_family, SocketType sock_type, SocketProtocol sock_proto>
    struct LookupRes
    {
        std::string address = "";

        Socket<SocketMode::PLAIN, sock_addr_family, sock_type, sock_proto> make_unsecure(){ return Socket<SocketMode::PLAIN, sock_addr_family, sock_type, sock_proto>(); }
        Socket<SocketMode::TLS, sock_addr_family, sock_type, sock_proto> make_secure(){ return Socket<SocketMode::TLS, sock_addr_family, sock_type, sock_proto>(); }
    };

    template<SocketAddressFamily sock_addr_family, SocketType sock_type = SocketType::STREAM, SocketProtocol sock_proto = SocketProtocol::TCP>
    Result<std::vector<LookupRes<sock_addr_family, sock_type, sock_proto>>> lookup(std::string_view domain_name)
    {
        LookupRes<sock_addr_family, sock_type, sock_proto> r;
        r.make_secure();

        struct addrinfo hints = {0};
        struct addrinfo* res = nullptr;

        hints.ai_family = (int)sock_addr_family;
        hints.ai_socktype = (int)sock_type;
        hints.ai_protocol = (int)sock_proto;

        if (getaddrinfo(domain_name.data(), NULL, &hints, &res) != 0)
        {
            logging::log_error("getaddrinfo failed: " + errors::get_err_str(errors::get_errno(true)));
            return {ResultCode::UNSUCCESSFUL, {}};
        }

        if (!res)
        {
            logging::log_error("what the sigma?? res is still null after getaddrinfo call!!??!?!!!!!!");
            return {ResultCode::UNSUCCESSFUL, {}}; 
        }

        std::vector<LookupRes<sock_addr_family, sock_type, sock_proto>> addrs;

        struct addrinfo* n = res;

        constexpr int buff_size = (sock_addr_family == SocketAddressFamily::IPV4) ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN;
        char buff[buff_size];

        while (n)
        {
            memset(buff, 0, buff_size);
            void* addr = nullptr;
            if constexpr (sock_addr_family == SocketAddressFamily::IPV4)
                addr = &((struct sockaddr_in*)n->ai_addr)->sin_addr;
            else if constexpr (sock_addr_family == SocketAddressFamily::IPV6)
                addr = &((struct sockaddr_in6*)n->ai_addr)->sin6_addr;

            if (inet_ntop(n->ai_family, addr, buff, buff_size) == NULL)
            {
                logging::log_error("inet_ntop failed: " + errors::get_err_str(errors::get_errno(true)));
                freeaddrinfo(res);
                return {ResultCode::UNSUCCESSFUL, {}};
            }

            addrs.push_back({buff});
            n = n->ai_next;
        }

        freeaddrinfo(res);

        return {ResultCode::SUCCESSFUL, addrs};
    }
}