cmake_minimum_required(VERSION 3.27)
project(2dGame)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

set(GLFW3_4_PATH "C:/Users/mjpea/.cpplibs/glfw-3.4.bin.WIN64")
set(GLEW_PATH "C:/Users/mjpea/.cpplibs/glew-2.1.0")

# GLFW include and library link
include_directories("${GLFW3_4_PATH}/include")
link_directories("${GLFW3_4_PATH}/lib-mingw-w64")

# https://www.glfw.org/docs/latest/build.html#build_link
find_package(OpenGL REQUIRED)

# GLEW include and library link (lib folder is emtpy for GLEW)
include_directories("${GLEW_PATH}/include")
link_directories("${GLEW_PATH}/lib/Release/x64")

add_compile_definitions(GLEW_STATIC)

add_executable(2dGame main.cpp
        renderer.h)

target_link_libraries(${PROJECT_NAME} glew32s GLFW3 OpenGL::GL)