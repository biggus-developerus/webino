#include <webino/webino.hpp>
#include <iostream>

using namespace webino;

int main()
{
    try 
    { 
        net::initialise(); 
        
        net::ClientSocket<net::AddressFamily::IPV4, net::SocketType::STREAM, net::SocketProtocol::TCP> socket;

        socket.connect(net::lookup<net::AddressFamily::IPV4>("x.com")[0].addr, 80);
        socket.send("GET / HTTP/1.1\r\nHost: x.com\r\n\r\n", 32);
        auto [data, size_recved] = socket.recv<char, 1024>();

        std::cout << std::string(data, size_recved) << std::endl;
        delete[] data;

        socket.close();
    }
    catch (const webino::errors::WebinoError& e) { std::cout << e.what() << std::endl;}
}