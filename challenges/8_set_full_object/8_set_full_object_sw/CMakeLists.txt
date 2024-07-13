cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "-pthread ")


add_definitions(-D_GNU_SOURCE=1)

file(GLOB chall_8_sources
    main.cpp
    )

project(chall_8)

add_executable(chall_8 ${chall_8_sources})

target_link_libraries(chall_8 sysrepo yang)


install(TARGETS chall_8 DESTINATION /usr/local/bin)