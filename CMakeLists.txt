cmake_minimum_required(VERSION 3.21)
project(MKwMG)
#external libraries
find_package(OpenGL REQUIRED)
if(WIN32)
    find_package(glew REQUIRED)
else()
    find_package(GLEW REQUIRED)
endif()
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(jsoncpp REQUIRED)
get_target_property(JSON_INC_PATH jsoncpp_lib INTERFACE_INCLUDE_DIRECTORIES)
if(WIN32)
    find_package(imgui CONFIG REQUIRED)
endif()
include_directories(MKwMG ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS}
        ${JSON_INC_PATH} "${GLM_BINARY_DIR}/include")

#libraries in repository
if(NOT WIN32)
    include_directories(MKwMG ${PROJECT_SOURCE_DIR} "lib/imgui")
    include_directories(MKwMG ${PROJECT_SOURCE_DIR} "lib/imgui/backends")
    include_directories(MKwMG ${PROJECT_SOURCE_DIR} "lib/imgui/misc/cpp")
    link_directories(MKwMG ${PROJECT_SOURCE_DIR} "lib/bin")
endif()
#include_directories(MKwMG ${PROJECT_SOURCE_DIR} "lib/valijson/include")
include_directories(MKwMG ${PROJECT_SOURCE_DIR} "lib/ImGui-Addons/FileBrowser")

#compile project
set(CMAKE_CXX_STANDARD 20)
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W2")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-common -Wall -Wextra -Wunused -Wsuggest-attribute=format -Wduplicated-cond -Wundef -Wstrict-null-sentinel -Wctor-dtor-privacy -Wnon-virtual-dtor -Wsuggest-final-types -Wsuggest-final-methods -Wsuggest-override -Wformat-overflow -Wformat-security -Wextra-semi -Wredundant-decls -Wnull-dereference -Wcast-align -Wshadow -Wfloat-equal -Wcast-qual -Wold-style-cast -Wuseless-cast -Wformat-signedness")
endif()
include_directories(MKwMG ${PROJECT_SOURCE_DIR}/lib ${PROJECT_SOURCE_DIR}/src)
file(GLOB_RECURSE MKWMG_SRC CONFIGURE_DEPENDS "src/*.h" "src/*.cpp")
add_executable(MKwMG "lib/ImGui-Addons/FileBrowser/ImGuiFileBrowser.cpp" ${MKWMG_SRC})
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

#set libraries
if(WIN32)
    target_link_libraries(MKwMG imgui::imgui)
else()
    target_link_libraries(MKwMG imgui)
endif()
target_link_libraries(MKwMG ${OPENGL_LIBRARY} ${GLFW3_LIBRARY} ${GLEW_LIBRARIES} glfw jsoncpp_lib)

