#pragma once

#include <string_view>
#include <string>
#include <optional>

namespace webino
{
    static const std::string URL_SCHEME_HTTP = "http";
    static const std::string URL_SCHEME_HTTPS = "https";
    
    struct Response
    {
        
    };

    std::optional<Response> get(std::string url); 
}