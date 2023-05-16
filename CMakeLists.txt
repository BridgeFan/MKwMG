cmake_minimum_required(VERSION 3.21)
project(MG1_zad2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lGLEW -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -fno-common -Wall -Wextra -Wunused -Wsuggest-attribute=format -Wduplicated-cond -Wundef -Wstrict-null-sentinel -Wctor-dtor-privacy -Wnon-virtual-dtor -Wsuggest-final-types -Wsuggest-final-methods -Wsuggest-override -Wformat-overflow -Wformat-security -Wextra-semi -Wredundant-decls -Wnull-dereference -Wcast-align -Wshadow -Wfloat-equal -Wcast-qual -Wold-style-cast -Wuseless-cast -Wformat-signedness")
include_directories(MG1_zad2 ${PROJECT_SOURCE_DIR} "lib/imgui")
include_directories(MG1_zad2 ${PROJECT_SOURCE_DIR} "src")
include_directories(MG1_zad2 ${PROJECT_SOURCE_DIR} "lib/valijson/include")
include_directories(MG1_zad2 ${PROJECT_SOURCE_DIR} "lib/jsoncpp/include")
include_directories(MG1_zad2 ${PROJECT_SOURCE_DIR} "lib/jsoncpp/src/lib_json")
include_directories(MG1_zad2 ${PROJECT_SOURCE_DIR} "lib/glm")
link_directories(MG1_zad2 ${PROJECT_SOURCE_DIR} "lib/bin")

set(HEADER_FILES)
set(CMAKE_CXX_STANDARD 20)

add_executable(MG1_zad2 
src/Solids/Solid.cpp
src/Solids/Torus.cpp
src/Util.cpp
src/Curves/BezierCurve2.cpp
src/Curves/BasicBezier.cpp
src/Curves/BezierCurveInter.cpp
src/Curves/BezierCurve0.cpp
src/Curves/BezierCommon.cpp
src/JsonUtil.cpp
src/Shader/ShaderArray.cpp
src/Shader/Shader.cpp
src/Surfaces/BezierSurface0.cpp
src/Surfaces/BezierSurfaceSegment0.cpp
src/Camera.cpp
src/Window.cpp
src/ImGui/ImGuiPanel.cpp
src/ImGui/ImGuiUtil.cpp
src/Object/Point.cpp
        src/Gizmos/Cursor.cpp
src/Object/Object.cpp
src/Object/ObjectArray.cpp
src/Object/Transform.cpp
        src/Gizmos/MultiCursor.cpp
src/Object/ObjectArrayListener.cpp
src/main.cpp
src/FileLoading.cpp
src/ConfigState.cpp
src/Scene.cpp
src/Event.h
src/Solids/Torus.h
src/Solids/Solid.h
src/Curves/BezierCurve2.h
src/Curves/BasicBezier.h
src/Curves/BezierCurve0.h
src/Curves/BezierCommon.h
src/Curves/BezierCurveInter.h
src/Shader/Shader.h
src/Shader/ShaderArray.h
src/Scene.h
src/ConfigState.h
src/FileLoading.h
src/Camera.h
src/Util.h
src/Surfaces/BezierSurfaceSegment0.h
src/Surfaces/BezierSurface0.h
src/JsonUtil.h
src/ImGui/ImGuiPanel.h
src/ImGui/ImGuiUtil.h
        src/ImGui/imgui_include.h
        src/Gizmos/MultiCursor.h
src/Object/Transform.h
src/Object/Object.h
src/Object/ObjectArray.h
src/Object/ObjectArrayListener.h
src/Object/Point.h
        src/Gizmos/Cursor.h
src/Window.h
        lib/ImGui-Addons/FileBrowser/ImGuiFileBrowser.cpp
src/Event.cpp src/Surfaces/BezierSurfaceCommon.cpp src/Surfaces/BezierSurfaceCommon.h src/library_include.h)
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

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

