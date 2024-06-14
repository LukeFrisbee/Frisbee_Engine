# Executable and Libraries
# file(GLOB sources _source/*.cpp)

set (sources 
    _source/board.cpp
)

# add_executable(Pins ${sources})
add_library(Pins ${sources})
target_include_directories(Pins PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/_include)

# Set C++ standard
if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET Pins PROPERTY CXX_STANDARD 20)
endif()

# Set to WIN32 Executable
#if(WIN32)
#    set_property(TARGET Pins PROPERTY WIN32_EXECUTABLE TRUE)
#endif()