#pragma once

#include <tuple>

namespace webino
{
    enum class ResultCode
    {
        SUCCESSFUL = 0,
        UNSUCCESSFUL = 1,
        SUCCESSFULISH = 2,
    };

    template <typename T>
    using Result = std::tuple<ResultCode, T>;

    template <typename T>
    inline bool is_successful(const Result<T>& res){ return std::get<0>(res) == ResultCode::SUCCESSFUL; }

    template <typename T>
    inline bool is_successfulish(const Result<T>& res){ return std::get<0>(res) == ResultCode::SUCCESSFULISH; }

    template <typename T>
    inline bool is_unsuccessful(const Result<T>& res){ return std::get<0>(res) == ResultCode::UNSUCCESSFUL; }

    template<typename T>
    inline T get_result(const Result<T>& res){ return std::get<1>(res); } // Should only be called after is_successful!!
}