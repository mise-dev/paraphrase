cmake_minimum_required(VERSION 3.5)
project(paraphrase_lite)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(OpenMP REQUIRED)
set(CMAKE_AUTOMOC ON)
add_subdirectory(QHotkey)
add_executable(a.out main.cpp)

target_include_directories(a.out PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp/include
  ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp/ggml/include 
  ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp/common
)
# Link Qt and CUDA libraries
target_link_libraries(a.out 
    Qt5::Core 
    Qt5::Gui 
    Qt5::Widgets
    qhotkey
    OpenMP::OpenMP_CXX
    ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp/build/src/libllama.a
	${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp/build/ggml/src/libggml.a
	${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp/build/ggml/src/libggml-base.a
	${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp/build/ggml/src/libggml-cpu.a
	pthread
	dl
    m 
)

