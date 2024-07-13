cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "-pthread ")


add_definitions(-D_GNU_SOURCE=1)

file(GLOB chall_9_sources
    main.cpp
    )

project(chall_9)

add_executable(chall_9 ${chall_9_sources})

target_link_libraries(chall_9 sysrepo yang)


install(TARGETS chall_9 DESTINATION /usr/local/bin)