#
# ImGui Library
#
cmake_minimum_required(VERSION 3.6.0)

include(CMakeOptions.cmake)
include(CMakeImplFiles.cmake)


##################################################################################################################
# Setup (change imgui version according to your needs)
##################################################################################################################
project(imgui
        VERSION 1.89.5
        LANGUAGES CXX
        DESCRIPTION "ImGui library."
)

#added
set(IMGUI_IMPL_GL3)
set(IMGUI_IMPL_GLFW)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
set(CMAKE_PDB_OUTPUT_DIRECTORY     ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)

set(CMAKE_VERBOSE_MAKEFILE TRUE)

if (WIN32)
  if (IMGUI_STATIC_LIBRARY)
    message(STATUS "[INFO] Building library as --> Static Library")
    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)
  else()
    message(STATUS "[INFO] Building library as --> DLL")
    set(BUILD_SHARED_LIBS ON)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
  endif()
endif()

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(EXAMPLES_DIR ${SOURCE_DIR}/examples)
set(BACKENDS_DIR ${SOURCE_DIR}/backends)
set(FONTS_DIR ${SOURCE_DIR}/misc/fonts)


##################################################################################################################
# Project Files
##################################################################################################################
set(HEADERS_CXX_FILES 
    ${SOURCE_DIR}/imgui.h
    ${SOURCE_DIR}/imconfig.h
    ${SOURCE_DIR}/imgui_internal.h
    ${SOURCE_DIR}/imstb_rectpack.h
    ${SOURCE_DIR}/imstb_textedit.h
    ${SOURCE_DIR}/imstb_truetype.h
    ${SOURCE_DIR}/misc/cpp/imgui_stdlib.h
)

set(SOURCES_CXX_FILES 
    ${SOURCE_DIR}/imgui.cpp
    ${SOURCE_DIR}/imgui_draw.cpp
    ${SOURCE_DIR}/imgui_tables.cpp
    ${SOURCE_DIR}/imgui_widgets.cpp
    ${SOURCE_DIR}/imgui_demo.cpp
    ${SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
)

file(GLOB FONTS_FILES ${FONTS_DIR}/*.ttf)

set(HEADERS_CXX_IMPL_FILES)
set(SOURCES_CXX_IMPL_FILES)
set(HEADERS_CXX_IMPL_FILES_2)
set(SOURCES_CXX_IMPL_FILES_2)
if (IMGUI_WITH_IMPL)
  Init_Impl_Files(HEADERS_CXX_IMPL_FILES HEADERS_CXX_IMPL_FILES_2 SOURCES_CXX_IMPL_FILES SOURCES_CXX_IMPL_FILES_2)
endif()


##################################################################################################################
# IDE Representation
##################################################################################################################
# Groups CMake pre-defined projects:
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

source_group("Header Files" FILES
    ${HEADERS_CXX_FILES}
)
source_group("Header Files\\Impl" FILES
    ${HEADERS_CXX_IMPL_FILES}
)
source_group("Source Files" FILES
    ${SOURCES_CXX_FILES}
)
source_group("Source Files\\Impl" FILES
    ${SOURCES_CXX_IMPL_FILES}
)
source_group("Resource Files" FILES
    ${FONTS_FILES}
)


##################################################################################################################
# Target
##################################################################################################################
add_library(${PROJECT_NAME} 
    ${HEADERS_CXX_FILES}
    ${SOURCES_CXX_FILES}
    ${HEADERS_CXX_IMPL_FILES}
    ${SOURCES_CXX_IMPL_FILES}
    ${HEADERS_CXX_IMPL_FILES_2}
    ${SOURCES_CXX_IMPL_FILES_2}
    ${FONTS_FILES}
)
target_include_directories(${PROJECT_NAME} PUBLIC 
    ${SOURCE_DIR}
    ${SOURCE_DIR}/examples
)
if(IMGUI_IMPL_DX12 AND NOT IMGUI_STATIC_LIBRARY)
    target_link_libraries(${PROJECT_NAME} PUBLIC d3d12)
endif()


##################################################################################################################
# Target Install
##################################################################################################################
set (DISTRIBUTE_DIR ${CMAKE_BINARY_DIR}/dist/$<CONFIG>)
install(
    TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${DISTRIBUTE_DIR}/bin
    LIBRARY DESTINATION ${DISTRIBUTE_DIR}/lib
    ARCHIVE DESTINATION ${DISTRIBUTE_DIR}/lib
)
install(
    FILES ${HEADERS_CXX_FILES} ${HEADERS_CXX_IMPL_FILES} ${HEADERS_CXX_IMPL_FILES_2}
    DESTINATION ${DISTRIBUTE_DIR}/include
)
install(
    FILES ${FONTS_FILES}
    DESTINATION ${DISTRIBUTE_DIR}/misc/fonts
)

# Install PDB files for VS builds to enable debugging:
if (MSVC)
  if (NOT IMGUI_STATIC_LIBRARY)
    install(
        FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> 
        DESTINATION ${DISTRIBUTE_DIR}/lib
        CONFIGURATIONS Debug OR RelWithDebInfo
        COMPONENT libraries
        OPTIONAL
    )
  else()
    install(
        FILES 
          $<$<CONFIG:Debug>:${CMAKE_BINARY_DIR}/bin/$<CONFIG>/${PROJECT_NAME}.pdb>
          $<$<CONFIG:RelWithDebInfo>:${CMAKE_BINARY_DIR}/${PROJECT_NAME}.dir/$<CONFIG>/${PROJECT_NAME}.pdb>
        DESTINATION ${DISTRIBUTE_DIR}/lib
        CONFIGURATIONS Debug OR RelWithDebInfo
        COMPONENT libraries
        OPTIONAL
    )
  endif()
endif()

