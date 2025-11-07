#include <iostream>
#include <type_traits>
#include <vector>
#include <list>
#include <tuple>

template<typename T, typename = std::enable_if<std::is_integral<T>::value>>
void print_ip(T v) {
    const size_t sz = sizeof(T);
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&v);
    for (size_t i = (sz - 1); i > 0; --i)
    {
        //unsigned char byte = (v >> (i * 8)) & 0xFF;
        std::cout << static_cast<int>(bytes[i]) << ".";
    }
    std::cout << static_cast<int>(bytes[0]) << std::endl;
} 

template<>
void print_ip(std::string v) {
    std::cout << v << std::endl;
}

template<>
void print_ip(std::vector<int> v) {
    if (v.size() > 1) {
        for (auto it = v.begin(); it < (v.end() - 1); ++it)
        {
            std::cout << *it << ".";
        }
        std::cout << *(v.end() - 1) << std::endl;
    } else if (v.size() == 1) {
        std::cout << *v.begin() << std::endl;
    } else {
        std::cout << "empty vector!" << std::endl;
    }
}

template<>
void print_ip(std::list<short> v) {
    if (v.size() > 1) {
        for (auto it = v.begin(); it != (--v.end()); ++it)
        {
            std::cout << *it << ".";
        }
        std::cout << *(--v.end()) << std::endl;
    } else if (v.size() == 1) {
        std::cout << *v.begin() << std::endl;
    } else {
        std::cout << "empty list!" << std::endl;
    }
}

template<typename T, std::size_t... idx>
void print_tuple(const T& t, std::index_sequence<idx...>) {
    size_t i = 0;
    ((std::cout << (i++ ? "." : "") << std::get<idx>(t)), ...);
    std::cout << std::endl;
}

template<typename... T>
void print_ip(std::tuple<T...> v) {
    /*
    size_t count = sizeof...(T);
    for (size_t i = 0; i < (count - 1); i++)
    {
        print_tuple_element(v, i);
    }*/
    if constexpr (std::conjunction_v<std::is_same<T, typename std::common_type_t<T...>>...>) {
        print_tuple(v, std::index_sequence_for<T...>{});
    }
}

/**
	\brief Main function of print_ip programm
	\author Dyagelev Roman
	\version 0.0.1
	\date 07.11.2025

	Main function for print_ip programm. This programm make for studying on otuscpp course and print ip
    with special print_ip() method, reloaded in templates.
    It can be integral digit, container(vector or list), std::string or tuple.
    Examples:
        print_ip( int8_t{-1} ); // 255 
        print_ip( int16_t{0} ); // 0.0
        print_ip( int32_t{2130706433} ); // 127.0.0.1 
        print_ip( int64_t{8875824491850138409} ); // 123.45.67.89.101.112.131.41 
        print_ip( std::string{"Hello, World!"} ); // Hello, World! 
        print_ip( std::vector<int>{100, 200, 300, 400} ); // 100.200.300.400 
        print_ip( std::vector<int>{100} );
        print_ip( std::vector<int>{} );
        print_ip( std::list<short>{400, 300, 200, 100} ); // 400.300.200.100 
        print_ip( std::list<short>{400} );
        print_ip( std::list<short>{} );
        print_ip( std::make_tuple(123, 456, 789, 0) ); // 123.456.789.0
*/
int main()
{
    print_ip( int8_t{-1} ); // 255 
    print_ip( int16_t{0} ); // 0.0
    print_ip( int32_t{2130706433} ); // 127.0.0.1 
    print_ip( int64_t{8875824491850138409} ); // 123.45.67.89.101.112.131.41 
    print_ip( std::string{"Hello, World!"} ); // Hello, World! 
    print_ip( std::vector<int>{100, 200, 300, 400} ); // 100.200.300.400 
    //print_ip( std::vector<int>{100} );
    //print_ip( std::vector<int>{} );
    print_ip( std::list<short>{400, 300, 200, 100} ); // 400.300.200.100 
    //print_ip( std::list<short>{400} );
    //print_ip( std::list<short>{} );
    print_ip( std::make_tuple(123, 456, 789, 0) ); // 123.456.789.0
    return 0;
}
