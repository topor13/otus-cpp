#include <iostream>
#include <map>
#include "my_list.hpp"
#include "my_alloc.hpp"

size_t factorial(size_t fac) {
    size_t res = 1;
    if (fac == 0) {
        return 1;
    }
    else {
        for (size_t i = 1; i <= fac; i++) {
            res *= i;
        }
    }

    return res;
}

int main()
{
    // create map with def allocator
    std::map<int, int> def_map;
    std::cout << "create default std::map<int, int>" << std::endl;
    for (size_t i = 0; i < 10; i++)
    {
        def_map.insert({i, factorial(i)});
    }
    for (std::map<int, int>::iterator it = def_map.begin(); it!= def_map.end(); ++it)
    {
        std::cout << it->first << " " << it->second << std::endl;
    }
    
    // create map with my custom allocator
    std::map<int, int, std::less<int>, MyFirstAllocator<std::pair<int, int>, 10>> my_map;
    std::cout << "create custom std::map<int, int>" << std::endl;
    for (size_t i = 0; i < 10; i++)
    {
        my_map.insert({i, factorial(i)});
    }
    for (std::map<int, int>::iterator it = my_map.begin(); it!= my_map.end(); ++it)
    {
        std::cout << it->first << " " << it->second << std::endl;
    }

    //create my custom list container with default allocator
    ListContainer<int> def_list;
    std::cout << "create custom list with default allocator" << std::endl;
    for (size_t i = 0; i < 10; i++)
    {
        def_list.push_back(i);
    }
    for (size_t i = 0; i < 10; i++)
    {
        std::cout << def_list[i] << std::endl;
    }

    //create my custom list container with my custom allocator
    ListContainer<int, MyFirstAllocator<int, 10>> my_alloc_list;
    std::cout << "create custom list with custom allocator" << std::endl;
    for (size_t i = 0; i < 10; i++)
    {
        my_alloc_list.push_back(i);
    }
    for (size_t i = 0; i < 10; i++)
    {
        std::cout << my_alloc_list[i] << std::endl;
    }


    return 0;
}
