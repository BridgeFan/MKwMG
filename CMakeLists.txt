cmake_minimum_required(VERSION 3.21)
project(MG1_zad2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lGLEW -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -Wall -Wextra -Wunused \
-Wsuggest-attribute=format -Wduplicated-cond -Wundef -Wnoexcept -Wstrict-null-sentinel \
-Wctor-dtor-privacy -Wnon-virtual-dtor -Wsuggest-final-types -Wsuggest-final-methods -Wsuggest-override \
-Wformat-overflow -Wformat-security -Wextra-semi -Wredundant-decls -Wnull-dereference \
-Wcast-align -Wshadow -fno-common -Wfloat-equal -Wcast-qual \
-Wold-style-cast -Wuseless-cast -Wformat-signedness")
include_directories(MG1_zad2 ${PROJECT_SOURCE_DIR} "lib")
include_directories(MG1_zad2 ${PROJECT_SOURCE_DIR} "src")
include_directories(MG1_zad2 ${PROJECT_SOURCE_DIR} "lib/Scene-Serializer-main/cpp/Serializer/Serializer")
link_directories(MG1_zad2 ${PROJECT_SOURCE_DIR} "lib/imgui-master")
link_directories(MG1_zad2 ${PROJECT_SOURCE_DIR} "lib/Scene-Serializer-main/cpp/Serializer")

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
        src/Object/Object.cpp
        src/Solids/Point.cpp
        src/Solids/Solid.cpp
        src/Solids/Torus.cpp
        src/Object/Transform.cpp
        src/Solids/MultiCursor.cpp
        src/Solids/MultiCursor.h
        src/Object/ObjectArray.cpp
        src/Object/ObjectArray.h
        src/Object/ObjectArrayListener.cpp
        src/Object/ObjectArrayListener.h
        src/Curves/BezierCurve.cpp
        src/Curves/BezierCurve.h
        src/Scene.cpp
        src/Scene.h
        src/Curves/BasicBezier.cpp
        src/Curves/BasicBezier.h
        src/Curves/BezierCurve2.cpp
        src/Curves/BezierCommon.cpp
        src/Curves/BezierCommon.h
        src/Curves/BezierCurveInter.cpp src/ShaderArray.cpp src/ShaderArray.h)

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
target_link_libraries(MG1_zad2 MGSerializer)
