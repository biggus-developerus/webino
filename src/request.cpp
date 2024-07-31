#include "webino/request.hpp"
#include "webino/errors.hpp"
#include "webino/net/net.hpp"

#include "webino/utils/str_utils.hpp"

#include <fmt/core.h>
#include <optional>
#include <iostream>
#include <variant>

namespace webino
{
    std::optional<Response> get(std::string url)
    {
        if (!net::_initialised)
            net::initialise();
            
        auto url_parts = utils::split(url, "://");
        if (url_parts.size() < 2)
            throw webino::errors::WebinoError("Invalid URL format");

        auto domain_parts = utils::split(url_parts[1], "/");
        if (domain_parts.size() < 2)   
            throw webino::errors::WebinoError("Invalid URL format");

        int port = 80;
        std::string_view scheme = url_parts[0];
        std::string_view domain = domain_parts[0];
        // std::string_view path = domain_parts[1];
        
        if (scheme == URL_SCHEME_HTTPS)
        {
            throw webino::errors::WebinoError("SSL is not supported yet... uwu!");
            port = 443;
        }

        auto sock = net::_connect(domain, port);
        if (!sock)
            throw webino::errors::WebinoError("");

        std::visit([&](auto& sock)
            {
                auto req = fmt::format("GET / HTTP/1.1\r\nHost: {}\r\n\r\n", domain);
                sock.send(req.data(), req.size());
                auto [data, size_recved] = sock.template recv<char, 1024>();

                std::cout << std::string(data, size_recved) << std::endl;
                delete[] data;

                sock.close();
            }, 
            sock.value()
        );

        return {};
    }
}