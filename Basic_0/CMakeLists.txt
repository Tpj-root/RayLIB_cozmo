cmake_minimum_required(VERSION 3.10)
project(basic_custom)

set(CMAKE_CXX_STANDARD 17)

# Raylib include and lib paths
include_directories(/home/when/Desktop/BUILD_FILES/raylib/build/raylib/include)
link_directories(/home/when/Desktop/BUILD_FILES/raylib/build/raylib)

# ======================================================
# 🧩 Custom build list — add your source and output names manually
# Format: add_executable(output_name source_file)
# ======================================================

add_executable(basic_0 main_0.cpp)
add_executable(basic_1 main_1.cpp)
#add_executable(basic_2 main_2.cpp)
add_executable(basic_3 main_3.cpp)
add_executable(basic_4 main_4.cpp)
add_executable(basic_5 main_5.cpp)
add_executable(basic_6 main_6.cpp)
add_executable(basic_7 main_7.cpp)

add_executable(8_basic main_8_basic.cpp)
add_executable(9_basic main_9.cpp)
add_executable(10_basic main_10.cpp)
add_executable(11_basic main_11.cpp)
add_executable(12_basic main_12.cpp)
add_executable(13_basic main_13.cpp)

# ======================================================
# 🔗 Link Raylib + system libraries to each target
# ======================================================

set(MY_LIBS raylib m pthread dl rt)

target_link_libraries(basic_0 ${MY_LIBS})
target_link_libraries(basic_1 ${MY_LIBS})
#target_link_libraries(basic_2 ${MY_LIBS})
target_link_libraries(basic_3 ${MY_LIBS})
target_link_libraries(basic_4 ${MY_LIBS})
target_link_libraries(basic_5 ${MY_LIBS})
target_link_libraries(basic_6 ${MY_LIBS})
target_link_libraries(basic_7 ${MY_LIBS})

target_link_libraries(8_basic ${MY_LIBS})
target_link_libraries(9_basic ${MY_LIBS})
target_link_libraries(10_basic ${MY_LIBS})
target_link_libraries(11_basic ${MY_LIBS})
target_link_libraries(12_basic ${MY_LIBS})
target_link_libraries(13_basic ${MY_LIBS})
