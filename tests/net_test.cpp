#include <webino/webino.hpp>
#include <iostream>

int main()
{
    if (!webino::is_successful(webino::initialise()))
        return 1;

    webino::net::Socket<webino::net::SocketMode::PLAIN> sock;
    if (!webino::is_successful(sock.connect("127.0.0.1", 443)))
        return 1;

    /*
    webino::get("https://www.google.com", headers, etc);
    webino::post("https://www.google.com", headers, data, etc);
    */
}