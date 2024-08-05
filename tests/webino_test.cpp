#include <webino/webino.hpp>
#include <iostream>

int main()
{
    if (!webino::is_successful(webino::initialise()))
        return 1;

    /*
    webino::get("https://www.google.com", headers, etc);
    webino::post("https://www.google.com", headers, data, etc);
    */
}