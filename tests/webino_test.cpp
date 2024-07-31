#include <webino/webino.hpp>
#include <iostream>

int main()
{    
    try { webino::get("http://google.com/hi"); }
    catch (const webino::errors::WebinoError& e) { std::cout << e.what() << std::endl;}
}