cmake_minimum_required(VERSION 3.20)
project(TactVis VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_executable(tactvis
    src/main.cpp
    src/renderer.cpp
    src/entity_manager.cpp
    src/sensor_receiver.cpp
    src/shader.cpp
    src/camera.cpp
    src/hud.cpp
    src/terrain.cpp
)

target_include_directories(tactvis PRIVATE
    include
    ${CMAKE_SOURCE_DIR}/vendor/glad/include
    ${CMAKE_SOURCE_DIR}/vendor/imgui
    ${CMAKE_SOURCE_DIR}/vendor/imgui/backends
)

target_sources(tactvis PRIVATE
    vendor/glad/src/glad.c
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/backends/imgui_impl_glfw.cpp
    vendor/imgui/backends/imgui_impl_opengl3.cpp
)

target_link_libraries(tactvis PRIVATE
    OpenGL::GL
    glfw
    glm::glm
)

if(APPLE)
    target_link_libraries(tactvis PRIVATE "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()

target_compile_options(tactvis PRIVATE -Wall -Wextra -O2)
