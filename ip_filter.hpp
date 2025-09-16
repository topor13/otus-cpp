#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#pragma once

// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]
std::vector<std::string> split(const std::string &str, char d)
{
    std::vector<std::string> r;

    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    while(stop != std::string::npos)
    {
        r.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(d, start);
    }

    r.push_back(str.substr(start));

    return r;
}

bool compare_ip(const std::vector<std::string>& first_ip, const std::vector<std::string>& second_ip)
{
    for (size_t i = 0; i < 4; ++i) {
        if (std::stoi(first_ip[i]) != std::stoi(second_ip[i])) {
            return std::stoi(first_ip[i]) > std::stoi(second_ip[i]);
        }
    }
    return false;
}

bool filter_ip(const std::vector<std::string> ip, int filter, int position = 4)
{
    if (position < 4 && position >= 0) {
        return std::stoi(ip[position]) == filter;
         
    } else {
        for (size_t i = 0; i < 4; ++i) {
            if (std::stoi(ip[i]) == filter) {
                return true;
            }
        }
    }

    return false;
}

void print_ip(std::vector<std::vector<std::string>>::const_iterator ip) {
    for(auto ip_part = ip->cbegin(); ip_part != ip->cend(); ++ip_part)
    {
        if (ip_part != ip->cbegin())
        {
            std::cout << ".";
        }
        std::cout << *ip_part;
    }
    std::cout << std::endl;
}
