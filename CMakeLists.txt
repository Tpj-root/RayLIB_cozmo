cmake_minimum_required(VERSION 3.10)
project(cozmo)

set(CMAKE_CXX_STANDARD 17)

# Raylib include and lib paths
include_directories(/home/when/Desktop/BUILD_FILES/raylib/build/raylib/include)
link_directories(/home/when/Desktop/BUILD_FILES/raylib/build/raylib)

# Source files
add_executable(cozmo
    main.cpp
)

# Link raylib
target_link_libraries(cozmo
    raylib
    m
    pthread
    dl
    rt
)

