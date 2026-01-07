cmake_minimum_required(VERSION 3.3)

project(eave VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

cmake_policy(SET CMP0072 NEW)
set(OpenGL_GL_PREFERENCE "GLVND")

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PULSE REQUIRED libpulse-simple)

file(GLOB_RECURSE VIS_SOURCES "src/vis/*.cpp")
file(GLOB_RECURSE AUD_SOURCES "src/aud/*.cpp")

set(SOURCE_FILES
    ${VIS_SOURCES}
    ${AUD_SOURCES}
    src/vis/glad.c
    src/aud/kiss_fft.c
    src/main.cpp
)

add_executable(eave ${SOURCE_FILES})

target_include_directories(eave PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    ${PULSE_INCLUDE_DIRS}
)

target_link_libraries(eave PRIVATE
    SDL2::SDL2
    OpenGL::GL
    ${PULSE_LIBRARIES}

)

install(TARGETS eave DESTINATION build)