#include <iostream>
#include <sstream>
#include <gtest/gtest.h>
#include "lib.h"
#include "ip_filter.hpp"

std::vector<std::vector<std::string>> ips = {{"10","11","12","13"},{"10","12","11","13"},{"13","11","12","10"},{"100","11","12","13"}};

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(OtusCppTest, checkVersion) {
    int ver_num = version();
    EXPECT_GT(ver_num, 0);
}

TEST(OtusCppTest, compareIp) {
    //compare_ip(std::vector<std::string> first_ip, std::vector<std::string> second_ip)
    //must return true if first_ip greater than second_ip 
    EXPECT_TRUE(compare_ip(ips[3],ips[0]));
}

TEST(OtusCppTest, filterIp) {
    EXPECT_TRUE(filter_ip(ips[3],100,0));
    EXPECT_FALSE(filter_ip(ips[3],100,1));
    EXPECT_TRUE(filter_ip(ips[3],100));
}

TEST(OtusCppTest, printIp) {
    std::stringstream buffer;
    std::streambuf* oldCout = std::cout.rdbuf(); // Сохраняем старый буфер
    std::cout.rdbuf(buffer.rdbuf()); // Перенаправляем cout в stringstream
    for (auto ip = ips.cbegin(); ip != ips.cend(); ++ip) {
        print_ip(ip);
    } 
    std::cout.rdbuf(oldCout); // Восстанавливаем исходный буфер
    std::string log_line = buffer.str();
    size_t pos1 = log_line.find("10.11.12.13");
    size_t pos2 = log_line.find("10.12.11.13");
    size_t pos3 = log_line.find("13.11.12.10");
    size_t pos4 = log_line.find("100.11.12.13");
    size_t pos5 = log_line.find("WRONG_STRING");
    ASSERT_TRUE(pos1 != std::string::npos);
    ASSERT_TRUE(pos2 != std::string::npos);
    ASSERT_TRUE(pos3 != std::string::npos);
    ASSERT_TRUE(pos4 != std::string::npos);
    ASSERT_TRUE(pos5 == std::string::npos);
}

TEST(OtusCppTest, sortIp) {
    std::sort(ips.begin(), ips.end(), compare_ip);
    ASSERT_EQ(ips[0][0], "100");
}