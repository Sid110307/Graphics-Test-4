cmake_minimum_required(VERSION 3.20)
project(GraphicsTest4)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    set(CMAKE_CXX_FLAGS "/W4 /WX")
else ()
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(graphicsTest4
        ${PROJECT_SOURCE_DIR}/main.cpp
        ${PROJECT_SOURCE_DIR}/shader.cpp
        ${PROJECT_SOURCE_DIR}/model.cpp
        ${PROJECT_SOURCE_DIR}/camera.cpp
        ${PROJECT_SOURCE_DIR}/objects.cpp
        ${PROJECT_SOURCE_DIR}/texture.cpp
)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(assimp REQUIRED)

target_sources(graphicsTest4 PUBLIC
        lib/imgui/imgui.cpp
        lib/imgui/imgui_demo.cpp
        lib/imgui/imgui_draw.cpp
        lib/imgui/imgui_tables.cpp
        lib/imgui/imgui_widgets.cpp
        lib/imgui/imgui_impl_glfw.cpp
        lib/imgui/imgui_impl_opengl3.cpp
)

target_link_libraries(graphicsTest4 PUBLIC ${CMAKE_DL_LIBS} glfw GLEW::GLEW OpenGL::GL assimp)
target_include_directories(graphicsTest4 PUBLIC lib)
