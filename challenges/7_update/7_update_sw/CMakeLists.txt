cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "-pthread ")


add_definitions(-D_GNU_SOURCE=1)

file(GLOB chall_7_sources
    main.cpp
    )

project(chall_7)

add_executable(chall_7 ${chall_7_sources})

target_link_libraries(chall_7 sysrepo yang)


install(TARGETS chall_7 DESTINATION /usr/local/bin)