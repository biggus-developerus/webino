#include <webino/webino.hpp>
#include <iostream>

int main()
{
    try 
    { 
        webino::initialise(); 
        for (auto res: webino::net::lookup<webino::net::AddressFamily::IPV6>("x.com"))
            std::cout << res.addr << "\n";
    }
    catch (const webino::errors::WebinoError& e) { std::cout << e.what() << std::endl;}
}