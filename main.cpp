#include <iostream>
#include <type_traits>
#include <vector>
#include <list>
#include <tuple>

//SFINAE for integrals
template<typename T>
auto print_ip(T v) -> std::enable_if_t<std::is_integral<T>::value> {
    const size_t sz = sizeof(T);
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&v);
    for (size_t i = (sz - 1); i > 0; --i)
    {
        //unsigned char byte = (v >> (i * 8)) & 0xFF;
        std::cout << static_cast<int>(bytes[i]) << ".";
    }
    std::cout << static_cast<int>(bytes[0]) << std::endl;
} 

// SFINAE for std::string without dots(std::string also container)
template<typename T>
auto print_ip(T v) -> std::enable_if_t<
        std::is_same_v<decltype(v), std::string>,
    void> {
    std::cout << v << std::endl;
}

// SFINAE for any container that have begin() method
// for list and vector containers
template<typename T>
auto print_ip(T v) -> std::enable_if_t<
        std::is_same_v<decltype(v.begin()), typename T::iterator> &&
        !std::is_same_v<decltype(v), std::string>,
    void> {
    if (v.size() > 1) {
        for (auto it = v.begin(); it != (--v.end()); ++it)
        {
            std::cout << *it << ".";
        }
        std::cout << *(--v.end()) << std::endl;
    } else if (v.size() == 1) {
        std::cout << *v.begin() << std::endl;
    } else {
        std::cout << "empty container!" << std::endl;
    }
}

template<typename T, std::size_t... idx>
void print_tuple(const T& t, std::index_sequence<idx...>) {
    size_t i = 0;
    ((std::cout << (i++ ? "." : "") << std::get<idx>(t)), ...);
    std::cout << std::endl;
}

// Only for tuple reload
template<typename... T>
auto print_ip(std::tuple<T...> v) -> std::enable_if_t<
        std::is_same_v<decltype(v), std::tuple<T...>>,
    void> {
    print_tuple(v, std::index_sequence_for<T...>{});
}

/**
	\brief Main function of print_ip programm
	\author Dyagelev Roman
	\version 0.0.1
	\date 07.11.2025

	Main function for print_ip programm. \n
    This programm make for studying on otuscpp course and print ip \n
    with special print_ip() method, reloaded in templates. \n
    It can be integral digit, container(vector or list), std::string or tuple. \n
    Examples:\n
        print_ip( int8_t{-1} ); // 255 \n
        print_ip( int16_t{0} ); // 0.0 \n
        print_ip( int32_t{2130706433} ); // 127.0.0.1 \n
        print_ip( int64_t{8875824491850138409} ); // 123.45.67.89.101.112.131.41 \n
        print_ip( std::string{"Hello, World!"} ); // Hello, World! \n
        print_ip( std::vector<int>{100, 200, 300, 400} ); // 100.200.300.400 \n
        print_ip( std::vector<int>{100} ); \n
        print_ip( std::vector<int>{} ); \n
        print_ip( std::list<short>{400, 300, 200, 100} ); // 400.300.200.100 \n
        print_ip( std::list<short>{400} ); \n
        print_ip( std::list<short>{} ); \n
        print_ip( std::make_tuple(123, 456, 789, 0) ); // 123.456.789.0 \n
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
