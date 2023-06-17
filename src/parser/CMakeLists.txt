add_library(lib_parser parser.cpp)
target_include_directories(lib_parser PUBLIC ${CMAKE_SOURCE_DIR}/src/parser ${CMAKE_SOURCE_DIR}/src/timeline ${CMAKE_SOURCE_DIR}/3rdparty/)
