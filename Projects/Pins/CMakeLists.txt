# Executable and Libraries
 file(GLOB sources _source/*.cpp)

set (sources
    _source/board.cpp
)

#set (sources_executable
#    _source/board.cpp
#    _source/main.cpp
#)

# Executable
#add_executable(PinsExecutable ${sources_executable})
#target_include_directories(PinsExecutable PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/_include)
#
#if (CMAKE_VERSION VERSION_GREATER 3.12)
#    set_property(TARGET PinsExecutable PROPERTY CXX_STANDARD 20)
#endif()

# Library
add_library(Pins ${sources})
target_include_directories(Pins PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/_include)

# Set C++ standard
if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET Pins PROPERTY CXX_STANDARD 20)
endif()