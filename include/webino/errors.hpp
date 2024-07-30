#pragma once

#include <string>
#include <exception>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <winsock2.h>
#else
    #include <cstring>
    #include <cerrno>
#endif

namespace webino::errors
{
    inline int get_errno(bool wsa = false) noexcept
    {
        #ifdef _WIN32
            if (wsa)
                return WSAGetLastError();
            return GetLastError();
        #endif
        return errno;
    }

    inline std::string get_err_str(int err = get_errno()) noexcept
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

    class WebinoError: public std::exception
    {
        public:
            WebinoError(std::string msg) : msg(msg) { }
            virtual const char* what() const noexcept { return this->msg.c_str(); }
        protected:
            std::string msg;
    };

    class WinSockError: public WebinoError
    {
        public:
            WinSockError(std::string msg): WebinoError(msg) { }
    };

    class SockError: public WebinoError
    {
        public:
            SockError(std::string msg): WebinoError(msg) { }
    };
}