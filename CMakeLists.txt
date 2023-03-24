cmake_minimum_required(VERSION 3.21)
project(MG1_zad2)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lGLEW -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lassimp -ljsoncpp -Wall -Wextra -llua5.3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lGLEW -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -Wall -Wextra -Wunused \
-Wsuggest-attribute=format -Wduplicated-cond -Wundef -Wnoexcept -Wstrict-null-sentinel \
-Wctor-dtor-privacy -Wnon-virtual-dtor -Wsuggest-final-types -Wsuggest-final-methods -Wsuggest-override \
-Wformat-overflow -Wformat-security -Wformat-signedness -Wextra-semi -Wredundant-decls -Wnull-dereference")
#-Wdouble-promotion -Wfloat-equal -Wuseless-cast
include_directories(Riku ${PROJECT_SOURCE_DIR} "imgui-master")
include_directories(Riku ${PROJECT_SOURCE_DIR} "imgui-master/backends")
include_directories(Riku ${PROJECT_SOURCE_DIR} "src")

set(HEADER_FILES)


set(CMAKE_CXX_STANDARD 20)

add_executable(MG1_zad2
        imgui-master/imgui.cpp
        imgui-master/imgui_draw.cpp
        imgui-master/imgui_tables.cpp
        imgui-master/imgui_widgets.cpp
        imgui-master/misc/cpp/imgui_stdlib.cpp
        imgui-master/backends/imgui_impl_glfw.cpp
        imgui-master/backends/imgui_impl_opengl3.cpp
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
        src/Solids/Transform.cpp src/Solids/MultiCursor.cpp src/Solids/MultiCursor.h)

target_link_libraries(MG1_zad2 dl)
target_link_libraries(MG1_zad2 Xi)
target_link_libraries(MG1_zad2 Xrandr)
target_link_libraries(MG1_zad2 pthread)
target_link_libraries(MG1_zad2 X11)
target_link_libraries(MG1_zad2 GL)
target_link_libraries(MG1_zad2 glfw)
target_link_libraries(MG1_zad2 GLEW)