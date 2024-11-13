#
# ImGui Implementation Files
#
cmake_minimum_required(VERSION 3.6.0)


function(Init_Impl_Files HEADERS_CXX_IMPL_FILES SOURCES_CXX_IMPL_FILES)
  if (IMGUI_IMPL_DX9)
    message(STATUS "[INFO] Including Direct3D9 implementation files.")
    set(HEADERS_CXX_IMPL_FILES
      ${EXAMPLES_DIR}/imgui_impl_dx9.h
      ${EXAMPLES_DIR}/imgui_impl_win32.h
      PARENT_SCOPE
    )
    set(SOURCES_CXX_IMPL_FILES
      ${EXAMPLES_DIR}/imgui_impl_dx9.cpp
      ${EXAMPLES_DIR}/imgui_impl_win32.cpp
      PARENT_SCOPE
    )
  elseif(IMGUI_IMPL_DX10)
    message(STATUS "[INFO] Including Direct3D10 implementation files.")
    set(HEADERS_CXX_IMPL_FILES
      ${EXAMPLES_DIR}/imgui_impl_dx10.h
      ${EXAMPLES_DIR}/imgui_impl_win32.h
      PARENT_SCOPE
    )
    set(SOURCES_CXX_IMPL_FILES
      ${EXAMPLES_DIR}/imgui_impl_dx10.cpp
      ${EXAMPLES_DIR}/imgui_impl_win32.cpp
      PARENT_SCOPE
    )
  elseif(IMGUI_IMPL_DX11)
    message(STATUS "[INFO] Including Direct3D11 implementation files.")
    set(HEADERS_CXX_IMPL_FILES
      ${EXAMPLES_DIR}/imgui_impl_dx11.h
      ${EXAMPLES_DIR}/imgui_impl_win32.h
      PARENT_SCOPE
    )
    set(SOURCES_CXX_IMPL_FILES
      ${EXAMPLES_DIR}/imgui_impl_dx11.cpp
      ${EXAMPLES_DIR}/imgui_impl_win32.cpp
      PARENT_SCOPE
    )
  elseif(IMGUI_IMPL_DX12)
    message(STATUS "[INFO] Including Direct3D12 implementation files.")
    set(HEADERS_CXX_IMPL_FILES
      ${EXAMPLES_DIR}/imgui_impl_dx12.h
      ${EXAMPLES_DIR}/imgui_impl_win32.h
      PARENT_SCOPE
    )
    set(SOURCES_CXX_IMPL_FILES
      ${EXAMPLES_DIR}/imgui_impl_dx12.cpp
      ${EXAMPLES_DIR}/imgui_impl_win32.cpp
      PARENT_SCOPE
    )
  else()
    if(IMGUI_IMPL_GL2)
      message(STATUS "[INFO] Including OpenGL2 implementation files.")
      set(HEADERS_CXX_IMPL_FILES ${BACKENDS_DIR}/imgui_impl_opengl2.h PARENT_SCOPE)
      set(SOURCES_CXX_IMPL_FILES ${BACKENDS_DIR}/imgui_impl_opengl2.cpp PARENT_SCOPE)
    elseif(IMGUI_IMPL_GL3)
      message(STATUS "[INFO] Including OpenGL3 implementation files.")
      set(HEADERS_CXX_IMPL_FILES ${BACKENDS_DIR}/imgui_impl_opengl3.h PARENT_SCOPE)
      set(SOURCES_CXX_IMPL_FILES ${BACKENDS_DIR}/imgui_impl_opengl3.cpp PARENT_SCOPE)
    else()
      message(WARNING "[WARNING] 'IMGUI_WITH_IMPL' option is ON but no additional files were added. See file 'CMakeOptions.cmake'.")
    endif()
    if(IMGUI_IMPL_GLFW)
      message(STATUS "[INFO] Including GLFW implementation files.")
      set(HEADERS_CXX_IMPL_FILES_2 ${BACKENDS_DIR}/imgui_impl_glfw.h PARENT_SCOPE)
      set(SOURCES_CXX_IMPL_FILES_2 ${BACKENDS_DIR}/imgui_impl_glfw.cpp PARENT_SCOPE)
    elseif(IMGUI_IMPL_GLUT)
      message(STATUS "[INFO] Including GLUT implementation files.")
      set(HEADERS_CXX_IMPL_FILES_2 ${BACKENDS_DIR}/imgui_impl_glut.h PARENT_SCOPE)
      set(SOURCES_CXX_IMPL_FILES_2 ${BACKENDS_DIR}/imgui_impl_glut.cpp PARENT_SCOPE)
    elseif(IMGUI_IMPL_SDL2)
      message(STATUS "[INFO] Including SDL2 implementation files.")
      set(HEADERS_CXX_IMPL_FILES_2 ${BACKENDS_DIR}/imgui_impl_sdl2.h PARENT_SCOPE)
      set(SOURCES_CXX_IMPL_FILES_2 ${BACKENDS_DIR}/imgui_impl_sdl2.cpp PARENT_SCOPE)
    elseif(IMGUI_IMPL_SDL3)
      message(STATUS "[INFO] Including SDL3 implementation files.")
      set(HEADERS_CXX_IMPL_FILES_2 ${BACKENDS_DIR}/imgui_impl_sdl3.h PARENT_SCOPE)
      set(SOURCES_CXX_IMPL_FILES_2 ${BACKENDS_DIR}/imgui_impl_sdl3.cpp PARENT_SCOPE)
    else()
    message(WARNING "[WARNING] 'IMGUI_WITH_IMPL' option is ON but no additional files were added. See file 'CMakeOptions.cmake'.")
    endif()
  endif()
endfunction()
