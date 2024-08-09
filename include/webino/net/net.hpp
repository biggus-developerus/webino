#pragma once

#include <openssl/ssl.h>

#include "enums.hpp"
#include "net_necessities.hpp"
#include "socket.hpp"
#include "_tls.hpp"

#include "webino/result.hpp"
#include "webino/logging.hpp"
#include "webino/errors.hpp"

namespace webino::net
{
    static bool _initialised = false;
    
    #ifdef _WIN32
        static WSADATA _wsa_data;
    #endif

    inline void _deinitialise()
    {
        ERR_free_strings();
        EVP_cleanup();
    }

    inline Result<void*> _initialise()
    {
        if (_initialised) return {ResultCode::SUCCESSFUL, nullptr};

        SSL_load_error_strings();
        SSL_library_init();
        OpenSSL_add_ssl_algorithms();

        atexit(_deinitialise);
        
        auto val = _new_ctx();
        if (!is_successful(val))
        {
            logging::log_error("Failed to create a new TLS context: " + errors::get_openssl_err_str());
            return {ResultCode::UNSUCCESSFUL, nullptr};
        }
        
        _client_ctx = get_result(val);

        #ifdef _WIN32
            int res = WSAStartup(MAKEWORD(WINSOCK_VERSION_MAJOR, WINSOCK_VERSION_MINOR), &_wsa_data);
            if (res != 0)
            {
                logging::log_critical("WSAStartup failed: " + errors::get_err_str(res));
                return {ResultCode::UNSUCCESSFUL, nullptr};
            }

            if (LOBYTE(_wsa_data.wVersion) != WINSOCK_VERSION_MAJOR || HIBYTE(_wsa_data.wVersion) != WINSOCK_VERSION_MINOR)
            {
                logging::log_critical(
                    "Couldn't find a usable winsock DLL. Expected version: " + 
                    std::to_string(WINSOCK_VERSION_MAJOR) + '.' + std::to_string(WINSOCK_VERSION_MINOR) + ", got: " + 
                    std::to_string((int)LOBYTE(_wsa_data.wVersion)) + '.' + std::to_string((int)HIBYTE(_wsa_data.wVersion))
                );

                return {ResultCode::UNSUCCESSFUL, nullptr};
            }
            _initialised = true;
        #else
            _initialised = true;
        #endif

        return {ResultCode::SUCCESSFUL, nullptr};
    }

}