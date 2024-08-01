#pragma once

#include "enums.hpp"
#include "lookup.hpp"

#include "webino/errors.hpp"

#include <iostream>
#include <optional>

namespace webino::net
{
    static bool _initialised = false;

    void initialise();
    void deinitialise();
}