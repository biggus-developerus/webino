#pragma once

#include <string>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <winsock2.h>
#else
    #include <cstring>
    #include <cerrno>
#endif

#include <openssl/err.h>

namespace webino::errors
{
    inline int get_errno(bool wsa = false)
    {
        #ifdef _WIN32
            if (wsa)
                return WSAGetLastError();
            return GetLastError();
        #endif
        return errno;
    }

    inline int get_openssl_err()
    {
        return ERR_get_error();
    }

    inline std::string get_openssl_err_str(int err = get_openssl_err())
    {
        std::string err_str;
        err_str.resize(256);
        ERR_error_string_n(err, err_str.data(), err_str.size());
        return err_str;
    }

    inline std::string get_err_str(int err = get_errno())
    {
        std::string err_str;
        #ifdef _WIN32
            char* buff = nullptr;
            int flags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
                        FORMAT_MESSAGE_FROM_SYSTEM | 
                        FORMAT_MESSAGE_IGNORE_INSERTS;
                        
            size_t buff_size = FormatMessage(
                flags, 
                NULL, 
                err, 
                LANG_NEUTRAL, 
                (LPSTR)&buff,
                0, 
                NULL
            ); 

            err_str = std::string(buff, buff_size);
        #else
            err_str = strerror(errno);
        #endif

        return err_str;
    }
}