cmake_minimum_required(VERSION 3.16)

set(CMAKE_BUILD_TYPE Debug)

# C++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Main executable
add_executable(timeline main.cpp)

add_subdirectory(src)

# Search for ncurses package
find_package(Curses REQUIRED)
set(CURSES_NEED_WIDE TRUE)

# Search for panels package
find_library(PANELS_LIBRARY NAMES panel)
target_include_directories(timeline PUBLIC ${CURSES_INCLUDE_DIR})

find_package(Threads REQUIRED)
find_package(FunctionalPlus REQUIRED)

#link necessary libraries
target_link_libraries(timeline ${CURSES_LIBRARIES} ${PANELS_LIBRARY} Threads::Threads FunctionalPlus::fplus lib_scrollable_window lib_timeline lib_parser)
