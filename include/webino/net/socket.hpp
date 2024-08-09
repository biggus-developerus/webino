#pragma once

#include <string_view>

#include "_tls.hpp"
#include "enums.hpp"
#include "net_necessities.hpp"

#include "webino/logging.hpp"
#include "webino/errors.hpp"
#include "webino/result.hpp"

namespace webino::net
{    
    template<
        SocketMode mode, 
        SocketAddressFamily address_family = SocketAddressFamily::IPV4, 
        SocketType socket_type = SocketType::STREAM, 
        SocketProtocol socket_protocol = SocketProtocol::TCP
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

                if (mode == SocketMode::TLS && !_client_ctx)
                {
                    logging::log_error("Attempted to create a TLS socket without a valid TLS context.");
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
                    
                    if (inet_pton(ipv6_addr->sin6_family, host_addr.data(), &ipv6_addr->sin6_addr) != 1)
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
                    logging::log_error("Got an unknown address family for address_family (" + std::to_string((int)address_family) + ")");
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

                if constexpr (mode == SocketMode::TLS)
                {
                    this->tls = SSL_new(_client_ctx);
                    if (!this->tls)
                    {
                        this->free_tls();
                        logging::log_error("Failed to create a new SSL structure: " + errors::get_openssl_err_str());
                        return {ResultCode::UNSUCCESSFUL, nullptr};
                    }

                    #pragma warning(push)
                    #pragma warning(disable : 4244)
                    SSL_set_fd(this->tls, this->sfd);
                    #pragma warning(pop)

                    if (SSL_connect(tls) != 1)
                    {
                        this->free_tls();
                        logging::log_error("TLS handshake failed: " + errors::get_openssl_err_str());
                        return {ResultCode::UNSUCCESSFUL, nullptr};
                    }
                }

                this->is_connected = true;
                
                return {ResultCode::SUCCESSFUL, nullptr};
            }

            Result<int> write(char* data, int data_size)
            {
                if (!this->is_valid() || !this->is_connected)
                {
                    logging::log_error("Attempted to write to an invalid/not connected socket");
                    return {ResultCode::UNSUCCESSFUL, -1};
                }

                int sent = -1;

                if constexpr (mode == SocketMode::TLS)
                {
                    if (!this->tls)
                    {
                        logging::log_error("Attempted to write to an invalid TLS socket");
                        return {ResultCode::UNSUCCESSFUL, -1};
                    }

                    if ((sent = SSL_write(this->tls, data, data_size)) <= 0)
                    {
                        int err = errors::get_openssl_err();
                        logging::log_error("SSL_write failed: " + errors::get_openssl_err_str(err));
                        return {ResultCode::UNSUCCESSFUL, err};
                    }

                    return {ResultCode::SUCCESSFUL, sent};
                }
                
                if ((sent = ::send(this->sfd, data, data_size, 0)) <= 0)
                {
                    int err = errors::get_errno(true);
                    logging::log_error("send failed: " + errors::get_err_str(err));
                    return {ResultCode::UNSUCCESSFUL, err};
                }

                return {ResultCode::SUCCESSFUL, sent};
            }

            Result<int> read(char* buffer, int buffer_size)
            {
                if (!this->is_valid() || !this->is_connected)
                {
                    logging::log_error("Attempted to read from an invalid/not connected socket");
                    return {ResultCode::UNSUCCESSFUL, -1};
                }

                int read = -1;

                if constexpr (mode == SocketMode::TLS)
                {
                    if (!this->tls)
                    {
                        logging::log_error("Attempted to read from an invalid TLS socket");
                        return {ResultCode::UNSUCCESSFUL, -1};
                    }

                    if ((read = SSL_read(this->tls, buffer, buffer_size)) <= 0)
                    {
                        int err = errors::get_openssl_err();
                        logging::log_error("SSL_read failed: " + errors::get_openssl_err_str(err));
                        return {ResultCode::UNSUCCESSFUL, err};
                    }

                    return {ResultCode::SUCCESSFUL, read};
                }

                if ((read = ::recv(this->sfd, buffer, buffer_size, 0)) <= 0)
                {
                    int err = errors::get_errno(true);
                    logging::log_error("read failed: " + errors::get_err_str(err));
                    return {ResultCode::UNSUCCESSFUL, err};
                }

                return {ResultCode::SUCCESSFUL, read};
            }

            Result<void*> close()
            {                
                this->free_tls();
                #ifdef _WIN32
                    ::closesocket(this->sfd);
                #else
                    ::close(this->sfd);
                #endif

                this->sfd = -1;
                this->is_connected = false;

                return {ResultCode::SUCCESSFUL, nullptr};
            }

            Result<void*> validate_cert(std::string_view domain_name = "")
            {
                if (!this->tls || !this->is_connected)
                {
                    logging::log_error("Attempted to validate hostname of an invalid/not connect socket.");
                    return {ResultCode::UNSUCCESSFUL, nullptr};
                }

                X509* cert = SSL_get_peer_certificate(this->tls);
                if (!cert)
                {
                    logging::log_error("No certificate was presented by the server.");
                    return {ResultCode::UNSUCCESSFUL, nullptr};
                }

                return _validate_cert(cert, domain_name);
            }

            inline bool is_valid() { return this->sfd != -1; }
        protected:
            SOCKET sfd = -1;
            SSL* tls = nullptr;
            bool is_connected = false;
        private:
            inline void free_tls()
            {
                if constexpr (mode != SocketMode::TLS)
                    return;

                if (this->tls)
                {
                    SSL_shutdown(this->tls);
                    SSL_free(this->tls);
                }

                this->tls = nullptr;
            }
    };
}