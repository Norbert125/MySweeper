cmake_minimum_required(VERSION 3.20)
project(Minesweeper)

set(CMAKE_CXX_STANDARD 17)

# --- 1. ENABLE RESOURCE COMPILER ---
# This tells CMake to use windres.exe (from MinGW) to handle the icon
enable_language(RC)

# 2. SFML Setup
set(SFML_DIR "E:/MySweeper/SFML-2.6.1/lib/cmake/SFML")
find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

# 3. Explicitly define the resource file path
set(RES_FILE "${CMAKE_CURRENT_SOURCE_DIR}/resources.rc")

# 4. Add files to the executable
add_executable(Minesweeper
        main.cpp
        Board.cpp
        Board.h
        ${RES_FILE}
)

# 5. Link SFML
target_link_libraries(Minesweeper sfml-graphics sfml-window sfml-system)