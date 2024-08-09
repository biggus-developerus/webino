#include <webino/webino.hpp>
#include <iostream>

int main()
{
    if (!webino::is_successful(webino::initialise()))
        return 1;

    webino::net::Socket<webino::net::SocketMode::TLS> sock;
    if (!webino::is_successful(sock.connect("127.0.0.1", 5'000)))
        return 1;

    if (webino::is_unsuccessful(sock.validate_cert("example.com")))
        return 1;

    if (!webino::is_successful(sock.write("GET / HTTP/1.1\r\nHost: example.com\r\n\r\n", 38)))
        return 1;

    char buff[1024] = {0};
    if (!webino::is_successful(sock.read(buff, 1024)))
        return 1;

    sock.close();

    std::cout << buff << "\n";

    webino::net::_free_client_ctx();
    webino::net::_deinitialise();

    /*
    webino::get("https://www.google.com", headers, etc);
    webino::post("https://www.google.com", headers, data, etc);
    */
}