cmake_minimum_required(VERSION 3.5)
project(paraphrase_lite)

# ----- Windows‑specific definitions -----
add_definitions(-D_WIN32_WINNT=0x0A00 -DNOMINMAX)

# ----- Qt5 setup (MXE provides its own Qt5) -----
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

# ----- QHotkey subdirectory (assumes it's cross‑compatible) -----
add_subdirectory(QHotkey)

# ----- OpenMP – let the compiler driver handle it -----
# Do NOT use find_package(OpenMP) – just add -fopenmp to flags
# The cross‑compiler will automatically link the correct libgomp from MXE

# ----- llama.cpp – make sure it's cross‑compiled for Windows! -----
# (if not, you'll get more "wrong format" errors)

add_executable(${PROJECT_NAME} WIN32 main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp/ggml/include
    ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp/common
)

target_compile_options(${PROJECT_NAME} PRIVATE -fopenmp)   # adds OpenMP
target_link_options(${PROJECT_NAME} PRIVATE -fopenmp)      # links libgomp

target_link_libraries(${PROJECT_NAME}
    Qt5::Core Qt5::Gui Qt5::Widgets
    qhotkey
    ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp/build/src/libllama.a   # MUST be Windows .a
    ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp/build/ggml/src/ggml.a
    ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp/build/ggml/src/ggml-base.a
    ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp/build/ggml/src/ggml-cpu.a
    # No pthread, dl, m – these are Unix libraries, not needed for Windows!
)
