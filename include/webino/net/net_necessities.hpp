#pragma once

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>

    #define WINSOCK_VERSION_MAJOR 2
    #define WINSOCK_VERSION_MINOR 2
#else
    #include <sys/socket.h>
    #include <unistd.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>

    typedef int SOCKET;
#endif