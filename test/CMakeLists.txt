cmake_minimum_required(VERSION 3.2)

find_package(GTest REQUIRED)

add_executable(testall 
                testall.cpp
                "../src/version.cpp"
                )
# add_executable(test_version test_version.cpp)

set_target_properties(testall PROPERTIES     
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(testall PRIVATE 
    ${PROJECT_SOURCE_DIR}/include
    "${PROJECT_SOURCE_DIR}/src"
    ${GTEST_INCLUDE_DIRS}
    "${CMAKE_BINARY_DIR}/include"
)
# set_target_properties(test_version PROPERTIES
#     COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
#     INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
# )

target_link_libraries( testall 
                        ${GTEST_LIBRARIES} 
                        Threads::Threads
                        )

# target_link_libraries(test_version
#     ${Boost_LIBRARIES}
#     helloworld
# )

if (MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    target_compile_options(testall PRIVATE
        /W4
    )
else ()
    target_compile_options(testall PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()

# enable_testing()

# add_test(testfull testall)
