cmake_minimum_required(VERSION 3.2)

if (DEFINED ENV{TRAVIS_BUILD_NUMBER})
    set (BUILD_VERSION $ENV{TRAVIS_BUILD_NUMBER})
else ()
    set (BUILD_VERSION 0)
endif()

project(ip_filter VERSION 1.0.${BUILD_VERSION})

add_subdirectory(src)
