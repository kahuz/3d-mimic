
cmake_minimum_required(VERSION 3.0.0)

project(3DMIMIC VERSION 0.1.0)

include_directories( #include files
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gl3w/
    ${CMAKE_CURRENT_SOURCE_DIR}/include/glfw/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/include/usyner
    ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/misc
) 

set( SOURCE # project source files

)
set( HEADER # include header source files
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gl3w/GL/gl3w
    ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/backends/imgui_impl_opengl2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/backends/imgui_impl_opengl3.cpp
)

if(WIN32) # if windows
    set(HEADER ${HEADER} ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/backends/imgui_impl_win32.cpp)
    if(CMAKE_CL_64) # if 64bit windows
        set(GLFW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/"/include/glfw/lib-vc2010-64")
    else() # if 32bit windows
        set(GLFW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/"/include/glfw/lib-vc2010-32")
    endif()
elseif(UNIX)
    add_compile_options(-Wall -Wextra)
endif()

add_executable(3DMIMIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${HEADER}
    ${SOURCE}
)

target_link_libraries(3DMIMIC 
    PUBLIC
    ${GLFW_DIR}/glfw3.lib opengl32.lib gdi32.lib
)