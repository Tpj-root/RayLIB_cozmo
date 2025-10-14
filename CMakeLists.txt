cmake_minimum_required(VERSION 3.10)
project(Cozmo)

set(CMAKE_CXX_STANDARD 17)

# Raylib include and lib paths
include_directories(/home/when/Desktop/BUILD_FILES/raylib/build/raylib/include)
link_directories(/home/when/Desktop/BUILD_FILES/raylib/build/raylib)

# Source files
add_executable(Cozmo
    main.cpp
)

# Link raylib
target_link_libraries(Cozmo
    raylib
    m
    pthread
    dl
    rt
)

