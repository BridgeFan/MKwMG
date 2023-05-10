# Install script for directory: /home/kamil-hp/Pulpit/MKwMG/lib

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/kamil-hp/Pulpit/MKwMG/lib/dist//lib/libimgui.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/kamil-hp/Pulpit/MKwMG/lib/dist//lib" TYPE STATIC_LIBRARY FILES "/home/kamil-hp/Pulpit/MKwMG/lib/bin/libimgui.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/kamil-hp/Pulpit/MKwMG/lib/dist//include/imgui.h;/home/kamil-hp/Pulpit/MKwMG/lib/dist//include/imconfig.h;/home/kamil-hp/Pulpit/MKwMG/lib/dist//include/imgui_internal.h;/home/kamil-hp/Pulpit/MKwMG/lib/dist//include/imstb_rectpack.h;/home/kamil-hp/Pulpit/MKwMG/lib/dist//include/imstb_textedit.h;/home/kamil-hp/Pulpit/MKwMG/lib/dist//include/imstb_truetype.h;/home/kamil-hp/Pulpit/MKwMG/lib/dist//include/imgui_stdlib.h;/home/kamil-hp/Pulpit/MKwMG/lib/dist//include/imgui_impl_opengl3.h;/home/kamil-hp/Pulpit/MKwMG/lib/dist//include/imgui_impl_glfw.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/kamil-hp/Pulpit/MKwMG/lib/dist//include" TYPE FILE FILES
    "/home/kamil-hp/Pulpit/MKwMG/lib/imgui/imgui.h"
    "/home/kamil-hp/Pulpit/MKwMG/lib/imgui/imconfig.h"
    "/home/kamil-hp/Pulpit/MKwMG/lib/imgui/imgui_internal.h"
    "/home/kamil-hp/Pulpit/MKwMG/lib/imgui/imstb_rectpack.h"
    "/home/kamil-hp/Pulpit/MKwMG/lib/imgui/imstb_textedit.h"
    "/home/kamil-hp/Pulpit/MKwMG/lib/imgui/imstb_truetype.h"
    "/home/kamil-hp/Pulpit/MKwMG/lib/imgui/misc/cpp/imgui_stdlib.h"
    "/home/kamil-hp/Pulpit/MKwMG/lib/imgui/backends/imgui_impl_opengl3.h"
    "/home/kamil-hp/Pulpit/MKwMG/lib/imgui/backends/imgui_impl_glfw.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/kamil-hp/Pulpit/MKwMG/lib/dist//misc/fonts/Cousine-Regular.ttf;/home/kamil-hp/Pulpit/MKwMG/lib/dist//misc/fonts/DroidSans.ttf;/home/kamil-hp/Pulpit/MKwMG/lib/dist//misc/fonts/Karla-Regular.ttf;/home/kamil-hp/Pulpit/MKwMG/lib/dist//misc/fonts/ProggyClean.ttf;/home/kamil-hp/Pulpit/MKwMG/lib/dist//misc/fonts/ProggyTiny.ttf;/home/kamil-hp/Pulpit/MKwMG/lib/dist//misc/fonts/Roboto-Medium.ttf")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/home/kamil-hp/Pulpit/MKwMG/lib/dist//misc/fonts" TYPE FILE FILES
    "/home/kamil-hp/Pulpit/MKwMG/lib/imgui/misc/fonts/Cousine-Regular.ttf"
    "/home/kamil-hp/Pulpit/MKwMG/lib/imgui/misc/fonts/DroidSans.ttf"
    "/home/kamil-hp/Pulpit/MKwMG/lib/imgui/misc/fonts/Karla-Regular.ttf"
    "/home/kamil-hp/Pulpit/MKwMG/lib/imgui/misc/fonts/ProggyClean.ttf"
    "/home/kamil-hp/Pulpit/MKwMG/lib/imgui/misc/fonts/ProggyTiny.ttf"
    "/home/kamil-hp/Pulpit/MKwMG/lib/imgui/misc/fonts/Roboto-Medium.ttf"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/kamil-hp/Pulpit/MKwMG/lib/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
