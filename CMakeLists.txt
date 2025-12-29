cmake_minimum_required(VERSION 3.20)
project(Minesweeper)

set(CMAKE_CXX_STANDARD 17)

# Enable Resource Compiler for your icon
enable_language(RC)

# 1. Path to SFML
set(SFML_DIR "E:/MySweeper/SFML-2.6.1/lib/cmake/SFML")

# 2. Add 'audio' to the COMPONENTS list here!
find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)

# 3. Resource file path
set(RES_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources.rc")

# 4. Add files
add_executable(Minesweeper
        main.cpp
        Board.cpp
        Board.h
        ${RES_FILE}
)

# 5. Add 'sfml-audio' to the link list here!
target_link_libraries(Minesweeper sfml-graphics sfml-window sfml-system sfml-audio)