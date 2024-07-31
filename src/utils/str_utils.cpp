#include "webino/utils/str_utils.hpp"
#include <iostream>

namespace webino::utils
{
    std::vector<std::string_view> split(const std::string_view str, const std::string_view delimiter) 
    {
        std::vector<std::string_view> res;

        size_t offset = 0;
        size_t pos = 0;

        while ((pos = str.find(delimiter, offset)) != std::string_view::npos)
        {
            res.push_back(str.substr(offset, pos - offset));
            offset = pos + delimiter.size();
        }

        res.push_back(str.substr(offset));

        return res;
    }
}