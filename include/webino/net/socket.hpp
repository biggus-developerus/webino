#pragma once

#include "net_necessities.hpp"
#include "enums.hpp"

#include "webino/logging.hpp"
#include "webino/errors.hpp"
#include "webino/result.hpp"

#include <string_view>

namespace webino::net
{
    template<
        SocketMode mode, 
        SocketAddressFamily address_family = SocketAddressFamily::IPV4, 
        SocketType socket_type = SocketType::STREAM, 
        SocketProtocol socket_protocol = SocketProtocol::ANY
    >
    class Socket
    {
        using _sock_t = Socket<mode, address_family, socket_type, socket_protocol>;
        public:
            Socket()
            {
                this->sfd = ::socket((int)address_family, (int)socket_type, (int)socket_protocol);
                if (this->sfd == -1)
                {
                    logging::log_error("Failed to create a new socket: " + errors::get_err_str(errors::get_errno(true)));
                    return;
                }
            }

            Result<void*> connect(std::string_view host_addr, int port)
            {
                if (!this->is_valid())
                {
                    logging::log_error("Attempted to connect an invalid socket");
                    return {ResultCode::UNSUCCESSFUL, nullptr};
                }

                sockaddr* addr = nullptr;
                int addr_size = 0;

                if constexpr (address_family == SocketAddressFamily::IPV4)
                {
                    sockaddr_in* ipv4_addr = new sockaddr_in();
                    std::memset(ipv4_addr, 0, sizeof(sockaddr_in));

                    ipv4_addr->sin_family = (int)address_family;
                    ipv4_addr->sin_port = htons(port);
                    
                    if (inet_pton(ipv4_addr->sin_family, host_addr.data(), &ipv4_addr->sin_addr) != 1)
                    {
                        logging::log_error("inet_pton failed: " + errors::get_err_str(errors::get_errno(true)));

                        delete ipv4_addr;
                        return {ResultCode::UNSUCCESSFUL, nullptr};
                    }

                    addr = (sockaddr*)ipv4_addr;
                    addr_size = sizeof(sockaddr_in);
                }
                else if constexpr (address_family == SocketAddressFamily::IPV6)
                {
                    sockaddr_in6* ipv6_addr = new sockaddr_in6();
                    ipv6_addr->sin6_family = (int)address_family;
                    ipv6_addr->sin6_port = htons(port);
                    
                    if (inet_pton(ipv6_addr->sin_family, host_addr.data(), &ipv6_addr->sin6_addr) != 1)
                    {
                        logging::log_error("inet_pton failed: " + errors::get_err_str(errors::get_errno(true)));

                        delete ipv6_addr;
                        return {ResultCode::UNSUCCESSFUL, nullptr};
                    }

                    addr = (sockaddr*)ipv6_addr;
                    addr_size = sizeof(sockaddr_in6);
                }
                else
                {
                    logging::log_error("Got an unknown address family for address_family (" + (int)address_family + ')');
                    return {ResultCode::UNSUCCESSFUL, nullptr};
                }

                int res = ::connect(this->sfd, addr, addr_size);

                if constexpr (address_family == SocketAddressFamily::IPV4)
                    delete (sockaddr_in*)addr;
                else if constexpr (address_family == SocketAddressFamily::IPV6)
                    delete (sockaddr_in6*)addr;
                else 
                    delete addr;

                if (res == -1)
                {
                    logging::log_error("Failed to connect to " + std::string(host_addr) + ':' + std::to_string(port) + ": " + errors::get_err_str(errors::get_errno(true)));
                    return {ResultCode::UNSUCCESSFUL, nullptr};
                }
                
                return {ResultCode::SUCCESSFUL, nullptr};
            }

            inline bool is_valid() { return this->sfd != -1; }
        protected:
            SOCKET sfd = -1;
    };
}