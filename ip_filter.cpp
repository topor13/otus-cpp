#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "ip_filter.hpp"

int main(int, char **)
{
    try
    {
        std::vector<std::vector<short>> ip_pool;

        for(std::string line; std::getline(std::cin, line);)
        {
            std::vector<std::string> v = split(line, '\t');
            std::vector<std::string> tmp_octets_str = split(v.at(0), '.');
            std::vector<short> tmp_octets_short;
            short tmp_oct;
            bool incorrect_ip = false;

            if(tmp_octets_str.size() > 4) {
                std::cerr << "ip must contain 4 digit!" << std::endl;
                continue;
            }

            for (auto oct = tmp_octets_str.cbegin(); oct != tmp_octets_str.cend(); ++oct)
            {
                tmp_oct = str_to_octet(*oct);
                if (tmp_oct < 0) {
                    incorrect_ip = true;
                    break;
                } else {
                    tmp_octets_short.push_back(tmp_oct);
                }
            }
            if (incorrect_ip) {
                std::cerr << "Wrong format of address! Must be four digit 0-255 delimit dot." << std::endl;
                continue;
            }
            ip_pool.push_back(tmp_octets_short);
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
