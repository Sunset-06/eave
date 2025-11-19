cmake_minimum_required(VERSION 3.10)

project(eav VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

add_executable(eav ${SOURCE_FILES})

target_include_directories(eav PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

install(TARGETS eav DESTINATION bin)