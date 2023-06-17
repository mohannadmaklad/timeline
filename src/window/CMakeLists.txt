
add_library(lib_scrollable_window
    "${CMAKE_SOURCE_DIR}/src/window/scrollableWindow.cpp"
    "${CMAKE_SOURCE_DIR}/src/window/utility.cpp"
)

# Search for panels package
find_library(PANELS_LIBRARY NAMES panel)

# Search for ncurses package
find_package(Curses REQUIRED)

#link necessary libraries
target_link_libraries(lib_scrollable_window ${CURSES_LIBRARIES} ${PANELS_LIBRARY} )
