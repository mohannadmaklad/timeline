
add_library(lib_timeline
    timeline.cpp)

target_include_directories(lib_timeline PUBLIC ${CMAKE_SOURCE_DIR}/src/window ${CMAKE_SOURCE_DIR}/3rdparty/)
target_link_libraries(lib_timeline lib_scrollable_window)