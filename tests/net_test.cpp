#include <webino/webino.hpp>
#include <iostream>

using namespace webino;

int main()
{
    if (!webino::is_successful(webino::initialise()))
        return 1;

    auto result = net::lookup<net::SocketAddressFamily::IPV4, net::SocketType::STREAM, net::SocketProtocol::TCP>("www.google.com");
    if (webino::is_unsuccessful(result))
        return 1;
    
    auto addr = webino::get_result(result)[0];
    auto sock = addr.make_secure();

    if (webino::is_unsuccessful(sock.connect(addr.address, 443)))
        return 1;

    if (webino::is_unsuccessful(sock.validate_cert("www.google.com")))
        return 1;

    if (webino::is_unsuccessful(sock.write("GET / HTTP/1.1\r\nHost: www.google.com\r\n\r\n", 41)))
        return 1;

    char buff[69420] = {0};
    auto res = sock.read(buff, 69420);
    if (webino::is_unsuccessful(res))
        return 1;

    std::cout << std::string(buff, webino::get_result(res)) << "\n";

    sock.close();

    // webino::net::_free_client_ctx();
    webino::net::deinitialise();

    /*
    webino::get("https://www.google.com", headers, etc);
    webino::post("https://www.google.com", headers, data, etc);
    */
}