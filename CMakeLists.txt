cmake_minimum_required(VERSION 3.21)
project(MKwMG)
#external libraries
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(jsoncpp REQUIRED)
get_target_property(JSON_INC_PATH jsoncpp_lib INTERFACE_INCLUDE_DIRECTORIES)
include_directories(MKwMG ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS}
        ${JSON_INC_PATH} "${GLM_BINARY_DIR}/include")

#libraries in repository
include_directories(MKwMG ${PROJECT_SOURCE_DIR} "lib/imgui")
include_directories(MKwMG ${PROJECT_SOURCE_DIR} "lib/valijson/include")
link_directories(MKwMG ${PROJECT_SOURCE_DIR} "lib/bin")

#compile project
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-common -Wall -Wextra -Wunused -Wsuggest-attribute=format -Wduplicated-cond -Wundef -Wstrict-null-sentinel -Wctor-dtor-privacy -Wnon-virtual-dtor -Wsuggest-final-types -Wsuggest-final-methods -Wsuggest-override -Wformat-overflow -Wformat-security -Wextra-semi -Wredundant-decls -Wnull-dereference -Wcast-align -Wshadow -Wfloat-equal -Wcast-qual -Wold-style-cast -Wuseless-cast -Wformat-signedness")
include_directories(MKwMG ${PROJECT_SOURCE_DIR}/lib ${PROJECT_SOURCE_DIR}/src)
file(GLOB_RECURSE MKWMG_SRC CONFIGURE_DEPENDS "src/*.h" "src/*.cpp")
add_executable(MKwMG "lib/ImGui-Addons/FileBrowser/ImGuiFileBrowser.cpp" ${MKWMG_SRC})
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

#set libraries
target_link_libraries(MKwMG imgui)
target_link_libraries(MKwMG ${OPENGL_LIBRARY} ${GLFW3_LIBRARY} ${GLEW_LIBRARIES} glfw jsoncpp_lib)

