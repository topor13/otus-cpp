#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <regex>

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

bool is_integer(const std::string & s){
    return std::regex_match(s, std::regex("[0-9]+"));
}

short str_to_octet(const std::string &str)
{
    if (!is_integer(str)) {
        return -3;
    }
    short oct;
    try {
        oct = std::stoi(str); // Преобразование в int
    } catch (const std::exception& e) {
        //std::cerr << "Неверный формат октета! Ошибка: " << e.what() << std::endl;
        return -2;
    }
    if (oct >=0 && oct < 256) {
        return oct;
    } else {
        //std::cerr << "Неверный формат октета! Значение должно быть в диапазоне 0-255, получено значение \"" << oct << "\"" << std::endl;
        return -1;
    }
}

bool compare_ip(const std::vector<short>& first_ip, const std::vector<short>& second_ip)
{
    for (size_t i = 0; i < 4; ++i) {
        if (first_ip[i] != second_ip[i]) {
            return first_ip[i] > second_ip[i];
        }
    }
    return false;
}

bool filter_ip(const std::vector<short> ip, int filter, int position = 4)
{
    if (position < 4 && position >= 0) {
        return ip[position] == filter;
         
    } else {
        for (size_t i = 0; i < 4; ++i) {
            if (ip[i] == filter) {
                return true;
            }
        }
    }

    return false;
}

void print_ip(std::vector<std::vector<short>>::const_iterator ip) {
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
