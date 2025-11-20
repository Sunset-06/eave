cmake_minimum_required(VERSION 3.3)

project(eave VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

cmake_policy(SET CMP0072 NEW)
set(OpenGL_GL_PREFERENCE "GLVND")

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
list(APPEND SOURCE_FILES "src/glad.c")

add_executable(eave ${SOURCE_FILES})

target_include_directories(eave PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(eave PRIVATE
    SDL2::SDL2
    OpenGL::GL 
)

install(TARGETS eave DESTINATION build)