#include <webino/webino.hpp>

int main()
{
    webino::logging::log_critical("test");
    webino::logging::log_error("test");
    webino::logging::log_warn("test");
    webino::logging::log_info("test");
    webino::logging::log_trace("test");
}