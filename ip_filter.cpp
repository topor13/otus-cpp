#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "ip_filter.hpp"

int main(int argc, char const *argv[])
{
    try
    {
        std::vector<std::vector<std::string> > ip_pool;

        for(std::string line; std::getline(std::cin, line);)
        {
            std::vector<std::string> v = split(line, '\t');
            ip_pool.push_back(split(v.at(0), '.'));
        }

        std::sort(ip_pool.begin(), ip_pool.end(), compare_ip);

        for(auto ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip)
        {
            print_ip(ip);
        }

        for(auto ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip)
        {
            if (filter_ip(*ip, 1, 0)) {
                print_ip(ip);
            }
        }

        for(auto ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip)
        {
            if (filter_ip(*ip, 46, 0) && filter_ip(*ip, 70, 1)) {
                print_ip(ip);
            }
        }

        for(auto ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip)
        {
            if (filter_ip(*ip, 46)) {
                print_ip(ip);
            }
        }
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
