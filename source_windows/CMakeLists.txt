cmake_minimum_required(VERSION 3.5)
project(paraphrase_lite)
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_CROSSCOMPILING TRUE)
add_subdirectory(QHotkey)
add_definitions(-D_WIN32_WINNT=0x0A00)  # Windows 10
add_definitions(-DNOMINMAX)  # Disable min/max macros
set(CMAKE_C_COMPILER /home/mise/b/mxe/usr/bin/x86_64-w64-mingw32.shared-gcc)
set(CMAKE_CXX_COMPILER /home/mise/b/mxe/usr/bin/x86_64-w64-mingw32.shared-g++)
set(CMAKE_RC_COMPILER /home/mise/b/mxe/usr/bin/x86_64-w64-mingw32.shared-windres)
set(CMAKE_SYSTEM_NAME Windows)
set(MXE_ROOT /home/mise/b/mxe)
set(MXE_TARGET ${MXE_ROOT}/usr/x86_64-w64-mingw32.shared)
set(QT5_MXE_PATH ${MXE_TARGET}/qt5)
list(APPEND CMAKE_PREFIX_PATH 
    ${QT5_MXE_PATH}}/qt5
    ${QT5_MXE_PATH}/qt5/lib/cmake
    ${QT5_MXE_PATH}}/qt5/lib/cmake/Qt5
    ${QT5_MXE_PATH}}/qt5/lib/cmake/Qt5Core
    ${QT5_MXE_PATH}/qt5/lib/cmake/Qt5Gui
    ${QT5_MXE_PATH}/qt5/lib/cmake/Qt5Widgets
)
set(Qt5_DIR ${QT5_MXE_PATH}/lib/cmake/Qt5)
set(Qt5Core_DIR ${QT5_MXE_PATH}/lib/cmake/Qt5Core)
set(Qt5Gui_DIR ${QT5_MXE_PATH}/lib/cmake/Qt5Gui)
set(Qt5Widgets_DIR ${QT5_MXE_PATH}/lib/cmake/Qt5Widgets)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(OpenMP_CXX_FOUND TRUE)
set(OpenMP_CXX_FLAGS "-fopenmp")
set(OpenMP_CXX_LIBRARIES "")
if(NOT TARGET OpenMP::OpenMP_CXX)
        add_library(OpenMP::OpenMP_CXX INTERFACE IMPORTED)
        set_target_properties(OpenMP::OpenMP_CXX PROPERTIES
            INTERFACE_COMPILE_OPTIONS "${OpenMP_CXX_FLAGS}"
            INTERFACE_LINK_LIBRARIES ""
        )
    endif()
    

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(Qt5_NO_SYSTEM_PATHS TRUE)
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(OpenMP REQUIRED)

add_executable(${PROJECT_NAME} WIN32 main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp/include
  ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp/ggml/include 
  ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp/common
)
# Link Qt and CUDA libraries
target_link_libraries(${PROJECT_NAME} 
    Qt5::Core 
    Qt5::Gui 
    Qt5::Widgets
    qhotkey
    OpenMP::OpenMP_CXX
    ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp/build/src/libllama.a
    ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp/build/ggml/src/ggml.a
    ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp/build/ggml/src/ggml-base.a
    ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp/build/ggml/src/ggml-cpu.a
	pthread
	dl
    m 
)
