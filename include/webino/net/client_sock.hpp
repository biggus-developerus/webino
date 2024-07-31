#pragma once

#include <variant>

#include "enums.hpp"

namespace webino::net
{
    template<AddressFamily addr_family, SocketType sock_type, SocketProtocol sock_proto>
    class ClientSocket
    {
        public:
            ClientSocket()
            {
                this->sfd = socket((int)addr_family, (int)sock_type, (int)sock_proto);

                if (this->sfd < 0)
                    throw webino::errors::SockError("Failed to create a socket: " + webino::errors::get_err_str(webino::errors::get_errno(true)));
            }

            ClientSocket(int sfd): sfd(sfd)
            {
                if (this->sfd < 0)
                    throw webino::errors::SockError("Attempted to instantiate an instance of webino::net::Socket with an invalid socket file descriptor.");
            }

            template<typename TOptVal, TOptVal default_val>
            TOptVal get_opt(OptionLevel opt_lvl, Option opt_name)
            {
                if (this->sfd < 0)
                    throw webino::errors::SockError("Attempted to get option of an invalid socket");
                    
                TOptVal val = default_val;
                socklen_t val_size = sizeof(TOptVal);

                if (getsockopt(this->sock_fd, (int)opt_lvl, (int)opt_name, (char*)&val, &val_size) == -1)
                    throw webino::errors::SockError("Failed to get socket option: " + webino::errors::get_err_str(webino::errors::get_errno(true)));
                return val;
            }

            void connect(std::string_view addr, int port)
            {
                if (this->sfd < 0)
                    throw webino::errors::SockError("Attempted to connect an invalid socket");

                sockaddr* sock_addr = nullptr;
                size_t addr_size = 0;

                if constexpr (addr_family == AddressFamily::IPV4)
                {
                    sockaddr_in ipv4_addr = {0};

                    ipv4_addr.sin_family = (int)addr_family;
                    ipv4_addr.sin_port = htons(port);

                    if (inet_pton(ipv4_addr.sin_family, addr.data(), &ipv4_addr.sin_addr.s_addr) <= 0)
                        throw webino::errors::SockError("Invalid IPv4 address or conversion error.");

                    sock_addr = (sockaddr*)&ipv4_addr;
                    addr_size = sizeof(ipv4_addr);
                }
                else if constexpr (addr_family == AddressFamily::IPV6)
                {
                    sockaddr_in6 ipv6_addr = {0};

                    ipv6_addr.sin6_family = (int)addr_family;
                    ipv6_addr.sin6_port = htons(port);

                    if (inet_pton(ipv6_addr.sin6_family, addr.data(), &ipv6_addr.sin6_addr) <= 0)
                        throw webino::errors::SockError("Invalid IPv6 address or conversion error.");
                    
                    sock_addr = (sockaddr*)&ipv6_addr;
                    addr_size = sizeof(ipv6_addr);
                }

                if (sock_addr == nullptr || addr_size == 0)
                    throw webino::errors::SockError("Failed to set sock_addr or addr_size");

                if (::connect(this->sfd, sock_addr, addr_size) != 0)
                    throw webino::errors::SockError("Failed to connect socket to " + std::string(addr) + ':' + std::to_string(port) + ", error: " + webino::errors::get_err_str(webino::errors::get_errno(true)));

                this->is_connected = true;
            }

            size_t send(const char* data, size_t data_size)
            {
                if (this->sfd < 0 || !this->is_connected)
                    throw webino::errors::SockError("Attempted to send data whilst not being connected.");

                size_t sent = ::send(this->sfd, data, data_size, 0);
                if (sent == -1)
                    throw webino::errors::SockError("Socket::send failed: " + webino::errors::get_err_str(webino::errors::get_errno(true)));

                return sent;
            }

            template<typename TBuff, size_t buff_size>
            std::tuple<TBuff*, size_t> recv()
            {  
                TBuff* buff = new TBuff[buff_size];
                if (this->sfd < 0 || !this->is_connected)
                    throw webino::errors::SockError("Attempted to recv data whilst not being connected.");

                size_t recved = ::recv(this->sfd, buff, buff_size, 0);
                if (recved == -1)
                    throw webino::errors::SockError("Socket::recv failed: " + webino::errors::get_err_str(webino::errors::get_errno(true)));

                return {buff, recved};
            }

            void close()
            {
                if (this->sfd < 0)
                    return;

                #ifdef _WIN32
                ::closesocket(this->sfd);
                #else
                ::close(this->sfd);
                #endif
            }
        protected:
            int sfd = -1;

            bool is_connected = false;
        private:

    };

    using TCPSocket = ClientSocket<AddressFamily::IPV4, SocketType::STREAM, SocketProtocol::TCP>;
    using UDPSocket = ClientSocket<AddressFamily::IPV4, SocketType::DGRAM, SocketProtocol::UDP>;
    using IPv6TCPSocket = ClientSocket<AddressFamily::IPV6, SocketType::STREAM, SocketProtocol::TCP>;
    using IPv6UDPSocket = ClientSocket<AddressFamily::IPV6, SocketType::DGRAM, SocketProtocol::UDP>;
    using SocketVariant = std::variant<TCPSocket, UDPSocket, IPv6TCPSocket, IPv6UDPSocket>;
}