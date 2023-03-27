cmake_minimum_required(VERSION 3.21)
project(MG1_zad2)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lGLEW -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lassimp -ljsoncpp -Wall -Wextra -llua5.3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lGLEW -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -Wall -Wextra -Wunused \
-Wsuggest-attribute=format -Wduplicated-cond -Wundef -Wnoexcept -Wstrict-null-sentinel \
-Wctor-dtor-privacy -Wnon-virtual-dtor -Wsuggest-final-types -Wsuggest-final-methods -Wsuggest-override \
-Wformat-overflow -Wformat-security -Wextra-semi -Wredundant-decls -Wnull-dereference \
-Woverloaded-virtual -Wcast-align -Wshadow -fno-common -Wfloat-equal -Wcast-qual \
-Wold-style-cast -Wuseless-cast -Wformat-signedness -Wdouble-promotion")
#include(${CMAKE_CURRENT_SOURCE_DIR}/imgui-master/CMakeLists.txt)
#include(imgui-master/cmake_install.cmake)
include_directories(MG1_zad2 ${PROJECT_SOURCE_DIR} "imgui-master")
include_directories(MG1_zad2 ${PROJECT_SOURCE_DIR} "imgui-master/backends")
include_directories(MG1_zad2 ${PROJECT_SOURCE_DIR} "src")
link_directories(MG1_zad2 ${PROJECT_SOURCE_DIR} "imgui-master")

set(HEADER_FILES)

set(CMAKE_CXX_STANDARD 20)

add_executable(MG1_zad2
        src/camera.cpp
        src/ImGuiUtil.cpp
        src/GlfwUtil.cpp
        src/Shader.cpp
        src/Util.cpp
        src/main.cpp
        src/Solids/Cursor.cpp
        src/Solids/Object.cpp
        src/Solids/Point.cpp
        src/Solids/Solid.cpp
        src/Solids/Torus.cpp
        src/Solids/Transform.cpp
        src/Solids/MultiCursor.cpp
        src/Solids/MultiCursor.h
        src/Solids/ObjectArray.cpp
        src/Solids/ObjectArray.h)

target_link_libraries(MG1_zad2 dl)
target_link_libraries(MG1_zad2 Xi)
target_link_libraries(MG1_zad2 Xrandr)
target_link_libraries(MG1_zad2 pthread)
target_link_libraries(MG1_zad2 X11)
target_link_libraries(MG1_zad2 GL)
target_link_libraries(MG1_zad2 glfw)
target_link_libraries(MG1_zad2 GLEW)
target_link_libraries(MG1_zad2 imgui_impl_glfw)
target_link_libraries(MG1_zad2 imgui_impl_opengl3)
target_link_libraries(MG1_zad2 imgui)