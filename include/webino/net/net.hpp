#pragma once

#include "enums.hpp"
#include "lookup.hpp"
#include "client_sock.hpp"

#include "webino/errors.hpp"

#include <iostream>
#include <optional>

namespace webino::net
{
    static bool _initialised = false;

    void initialise();
    void deinitialise();

    std::optional<SocketVariant> _connect(std::string_view domain, int port);
    
    template<AddressFamily addr_family, SocketType sock_type, SocketProtocol sock_proto>
    inline std::optional<ClientSocket<addr_family, sock_type, sock_proto>> _new_sock()
    {
        return ClientSocket<addr_family, sock_type, sock_proto>(); 
        // catch (const webino::errors::WebinoError& e) { return std::nullopt; }
    }

    inline std::optional<SocketVariant> _get_appropriate_sock(LookupRes& res)
    {
        // on skibski... wtf is this... 😔😔
        std::cout << res.addr << "\n";
        if (res.sock_proto == SocketProtocol::ANY || res.sock_type == SocketType::ANY)
            throw webino::errors::WebinoError("Please kill yourself and fill in the fucking hints you lazy bitch");

        if (res.addr_family == AddressFamily::IPV4)
        {
            if (res.sock_type == SocketType::STREAM && res.sock_proto == SocketProtocol::TCP)
                return _new_sock<AddressFamily::IPV4, SocketType::STREAM, SocketProtocol::TCP>();
            else if (res.sock_type == SocketType::DGRAM && res.sock_proto == SocketProtocol::UDP)
                return _new_sock<AddressFamily::IPV4, SocketType::DGRAM, SocketProtocol::UDP>();
        }
        else if (res.addr_family == AddressFamily::IPV6)
        {
            if (res.sock_type == SocketType::STREAM && res.sock_proto == SocketProtocol::TCP)
                return _new_sock<AddressFamily::IPV6, SocketType::STREAM, SocketProtocol::TCP>();
            else if (res.sock_type == SocketType::DGRAM && res.sock_proto == SocketProtocol::UDP)
                return _new_sock<AddressFamily::IPV6, SocketType::DGRAM, SocketProtocol::UDP>();
        }        
        return std::nullopt;
    }
}