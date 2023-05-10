#!/bin/bash
# shellcheck disable=SC2069
exec 2>&1 1>CMakeLists.txt

echo "cmake_minimum_required(VERSION 3.21)
project(MG1_zad2)

set(CMAKE_CXX_FLAGS \"\${CMAKE_CXX_FLAGS} -lGLEW -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -fno-common \
-Wall -Wextra -Wunused \
-Wsuggest-attribute=format -Wduplicated-cond -Wundef -Wnoexcept -Wstrict-null-sentinel \
-Wctor-dtor-privacy -Wnon-virtual-dtor -Wsuggest-final-types -Wsuggest-final-methods -Wsuggest-override \
-Wformat-overflow -Wformat-security -Wextra-semi -Wredundant-decls -Wnull-dereference \
-Wcast-align -Wshadow -Wfloat-equal -Wcast-qual \
-Wold-style-cast -Wuseless-cast -Wformat-signedness\")
include_directories(MG1_zad2 \${PROJECT_SOURCE_DIR} \"lib\")
include_directories(MG1_zad2 \${PROJECT_SOURCE_DIR} \"src\")
include_directories(MG1_zad2 \${PROJECT_SOURCE_DIR} \"/lib/imgui\")
include_directories(MG1_zad2 \${PROJECT_SOURCE_DIR} \"/usr/include/jsoncpp\")
link_directories(MG1_zad2 \${PROJECT_SOURCE_DIR} \"lib/bin\")

set(HEADER_FILES)
set(CMAKE_CXX_STANDARD 20)

add_executable(MG1_zad2 "
find src -name "*.cpp"
find src -name "*.h"

echo ")

target_link_libraries(MG1_zad2 dl)
target_link_libraries(MG1_zad2 Xi)
target_link_libraries(MG1_zad2 Xrandr)
target_link_libraries(MG1_zad2 pthread)
target_link_libraries(MG1_zad2 X11)
target_link_libraries(MG1_zad2 GL)
target_link_libraries(MG1_zad2 glfw)
target_link_libraries(MG1_zad2 GLEW)
target_link_libraries(MG1_zad2 imgui)
target_link_libraries(MG1_zad2 jsoncpp)
"
