cmake_minimum_required(VERSION 3.21)
project(MKwMG)
if(WIN32)
    list(APPEND CMAKE_PREFIX_PATH "C:/msys64/mingw64/lib/cmake")
    set(glm_DIR "C:/msys64/mingw64/lib/cmake/glm")
endif()
#external libraries
find_package(OpenGL REQUIRED)
if(WIN32)
    find_package(glew REQUIRED)
else()
    find_package(GLEW REQUIRED)
endif()
find_package(glfw3 REQUIRED)

#find_package(glm CONFIG REQUIRED) - TODO: on my own risk
find_package(jsoncpp REQUIRED)
get_target_property(JSON_INC_PATH jsoncpp_lib INTERFACE_INCLUDE_DIRECTORIES)
#if(WIN32)
#    find_package(imgui CONFIG REQUIRED)
#endif()
include_directories(MKwMG ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS}
        ${JSON_INC_PATH} "${GLM_BINARY_DIR}/include")

#libraries in repository
#if(NOT WIN32)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR} "lib/imgui")
include_directories(SYSTEM ${PROJECT_SOURCE_DIR} "lib/imgui/backends")
include_directories(SYSTEM ${PROJECT_SOURCE_DIR} "lib/imgui/misc/cpp")
include_directories(SYSTEM ${PROJECT_SOURCE_DIR} "lib/valijson/include")
link_directories(SYSTEM ${PROJECT_SOURCE_DIR} "lib/bin")
#endif()
include_directories(MKwMG PRIVATE ${VALIJSON_INCLUDE_DIRS})
include_directories(MKwMG ${PROJECT_SOURCE_DIR} "lib")

#compile project
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-common -Wall -Wextra -Wunused -Wsuggest-attribute=format -Wduplicated-cond -Wundef -Wstrict-null-sentinel -Wctor-dtor-privacy -Wnon-virtual-dtor -Wsuggest-final-types -Wsuggest-final-methods -Wsuggest-override -Wformat-overflow -Wformat-security -Wextra-semi -Wredundant-decls -Wnull-dereference -Wcast-align -Wshadow -Wfloat-equal -Wcast-qual -Wold-style-cast -Wuseless-cast -Wformat-signedness")
include_directories(MKwMG ${PROJECT_SOURCE_DIR}/lib ${PROJECT_SOURCE_DIR}/src)
file(GLOB_RECURSE MKWMG_SRC CONFIGURE_DEPENDS "src/*.h" "src/*.cpp")
if(WIN32)
    list(APPEND MKWMG_SRC "lib/imgui/backends/imgui_impl_opengl3.cpp" "lib/imgui/backends/imgui_impl_glfw.cpp" "lib/imgui/misc/cpp/imgui_stdlib.cpp")
    list(APPEND MKWMG_SRC "lib/imgui/imgui.cpp" "lib/imgui/imgui_draw.cpp" "lib/imgui/imgui_tables.cpp" "lib/imgui/imgui_widgets.cpp")
endif()
add_executable(MKwMG ${MKWMG_SRC} src/Object/EquidistanceSurface.h)
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

#set libraries
if(NOT WIN32)
    target_link_libraries(MKwMG imgui)
endif()
target_link_libraries(MKwMG ${OPENGL_LIBRARY} ${GLFW3_LIBRARY} ${GLEW_LIBRARIES} glfw jsoncpp_lib)

