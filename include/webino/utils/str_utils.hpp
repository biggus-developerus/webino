#pragma once

#include <vector>
#include <string_view>

namespace webino::utils
{
    std::vector<std::string_view> split(const std::string_view str, const std::string_view delimiter);
}