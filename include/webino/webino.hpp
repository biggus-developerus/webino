#pragma once

#include "result.hpp"
#include "logging.hpp"
#include "net/net.hpp"

namespace webino
{
    inline Result<void*> initialise()
    {
        if (!is_successful(net::_initialise()))
            return {ResultCode::UNSUCCESSFUL, nullptr};

        return {ResultCode::SUCCESSFUL, nullptr};
    }
}