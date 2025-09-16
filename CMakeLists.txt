cmake_minimum_required(VERSION 3.12)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(helloworld VERSION ${PROJECT_VERSION})

option(WITH_BOOST_TEST "Whether to build Boost test" ON)
set(WITH_GOOGLE_TESTS ON)

configure_file(version.h.in version.h)

add_executable(ip_filter ip_filter.cpp ip_filter.hpp)

add_executable(helloworld_cli main.cpp)
add_library(helloworld lib.cpp)

set_target_properties(helloworld_cli helloworld ip_filter PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(helloworld
    PRIVATE "${CMAKE_BINARY_DIR}"
)

target_link_libraries(helloworld_cli PRIVATE
    helloworld
)

if(WITH_BOOST_TEST)
    find_package(Boost COMPONENTS unit_test_framework REQUIRED)
    add_executable(boost_otus_cpp_tests tests/boost_otus_cpp_tests.cpp)

    set_target_properties(boost_otus_cpp_tests PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
    )

    set_target_properties(boost_otus_cpp_tests PROPERTIES
        COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
        INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
    )

    target_include_directories(
        boost_otus_cpp_tests PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
    )

    target_link_libraries(boost_otus_cpp_tests
        ${Boost_LIBRARIES}
        helloworld
    )
endif()

if(WITH_GOOGLE_TESTS)
# Build googletest
add_subdirectory(
    #"${CMAKE_CURRENT_SOURCE_DIR}/googletest" 
    "googletest"
)

# Test target
add_executable(
    g_otus_cpp_tests
    tests/g_otus_cpp_tests.cpp
)
# Add src to the include directories for the test target
target_include_directories(
    g_otus_cpp_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Link test target against gtest libraries
target_link_libraries(
    g_otus_cpp_tests PRIVATE
    gtest
	helloworld
)
endif()

if (MSVC)
    target_compile_options(helloworld_cli PRIVATE
        /W4
    )
    target_compile_options(helloworld PRIVATE
        /W4
    )
    if(WITH_BOOST_TEST)
        target_compile_options(boost_otus_cpp_tests PRIVATE
            /W4
        )
    endif()
else ()
    target_compile_options(helloworld_cli PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(helloworld PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    if(WITH_BOOST_TEST)
        target_compile_options(boost_otus_cpp_tests PRIVATE
            -Wall -Wextra -pedantic -Werror
        )
    endif()
endif()

install(TARGETS helloworld_cli RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)

if(WITH_BOOST_TEST)
    enable_testing()
    add_test(test_version boost_otus_cpp_tests)
endif()

if(WITH_GOOGLE_TESTS)
    enable_testing()
    add_test(checkVersion g_otus_cpp_tests)
endif()