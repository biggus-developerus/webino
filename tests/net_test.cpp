#include <webino/webino.hpp>
#include <iostream>

using namespace webino;

int main()
{
    if (!webino::is_successful(webino::initialise()))
        return 1;

    auto result = net::lookup<net::SocketAddressFamily::IPV4, net::SocketType::STREAM, net::SocketProtocol::TCP>("example.com");

    if (webino::is_unsuccessful(result))
        return 1;
    
    auto addr = webino::get_result(result)[0];
    auto sock = addr.make_secure();

    if (webino::is_unsuccessful(sock.connect(addr.address, 443)))
        return 1;

    if (webino::is_unsuccessful(sock.validate_cert("example.com")))
        return 1;

    if (webino::is_unsuccessful(sock.write("GET / HTTP/1.1\r\nHost: example.com\r\n\r\n", 38)))
        return 1;

    char buff[1024] = {0};
    if (webino::is_unsuccessful(sock.read(buff, 1024)))
        return 1;

    sock.close();

    std::cout << buff << "\n";
    webino::net::_deinitialise();

    /*
    webino::get("https://www.google.com", headers, etc);
    webino::post("https://www.google.com", headers, data, etc);
    */
}