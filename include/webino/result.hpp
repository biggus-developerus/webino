#pragma once

#include <tuple>

namespace webino
{
    enum class ResultCode
    {
        SUCCESSFUL = 0,
        UNSUCCESSFUL = 1
    };

    template <typename T>
    using Result = std::tuple<ResultCode, T>;

    template <typename T>
    inline bool is_successful(const Result<T>& res){ return std::get<0>(res) == ResultCode::SUCCESSFUL; }
}