cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "-pthread ")


add_definitions(-D_GNU_SOURCE=1)

file(GLOB chall_10_sources
    main.cpp
    )

project(chall_10)

add_executable(chall_10 ${chall_10_sources})

target_link_libraries(chall_10 sysrepo yang)


install(TARGETS chall_10 DESTINATION /usr/local/bin)