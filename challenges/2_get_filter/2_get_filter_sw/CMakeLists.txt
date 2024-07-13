cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "-pthread ")


add_definitions(-D_GNU_SOURCE=1)

file(GLOB chall_2_sources
    main.cpp
    )

project(chall_2)

add_executable(chall_2 ${chall_2_sources})

target_link_libraries(chall_2 sysrepo yang)


install(TARGETS chall_2 DESTINATION /usr/local/bin)