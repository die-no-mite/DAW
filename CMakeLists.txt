cmake_minimum_required(VERSION 4.1)
project(DAW)

set(CMAKE_CXX_STANDARD 20)

add_executable(DAW main.cpp)

target_link_libraries(DAW PRIVATE
        opengl32
        gdiplus
        dwmapi
        shlwapi
        winmm
)
