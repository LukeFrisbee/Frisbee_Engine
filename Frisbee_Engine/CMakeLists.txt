# SHADERS
# Define shader source file paths
file(GLOB_RECURSE SHADER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert" "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag")

# Add custom commands and targets for shader compilation
foreach(SHADER_SOURCE ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER_SOURCE} NAME)
    set(SHADER_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER_NAME}.spv")
    add_custom_command(
        OUTPUT ${SHADER_OUTPUT}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/shaders
        COMMAND glslc -o ${SHADER_OUTPUT} ${SHADER_SOURCE}
        DEPENDS ${SHADER_SOURCE}
        COMMENT "Compiling ${SHADER_NAME}"
    )
    list(APPEND SHADER_OUTPUTS ${SHADER_OUTPUT})
endforeach()

add_custom_target(ShadersTarget DEPENDS ${SHADER_OUTPUTS})
# ^ SHADERS

# GLM
include_directories("glm")

# TinyObjLoad
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tinyobjloader)

# GLFW
set(GLFW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.3.9)
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
add_subdirectory(${GLFW_DIR} binary_dir EXCLUDE_FROM_ALL)
include_directories(${GLFW_DIR}/include)
include_directories(${GLFW_DIR}/deps)

# VULKAN
find_package(Vulkan REQUIRED)
set(LIBRARIES "glfw;Vulkan::Vulkan")

# IMGUI
set(IMGUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
file(GLOB IMGUI_SOURCES ${IMGUI_INCLUDE_DIR}/*.cpp ${IMGUI_INCLUDE_DIR}/*.h) 
add_library("imgui" STATIC ${IMGUI_SOURCES})
target_include_directories("imgui" PUBLIC ${IMGUI_INCLUDE_DIR})
target_link_libraries("imgui" PRIVATE ${LIBRARIES})

# Executable and Libraries
file(GLOB sources _src/*.cpp _src/*.h)
add_executable(Frisbee_Engine ${sources})

# Make sure shader compilation occurs before building the main target
add_dependencies(Frisbee_Engine ShadersTarget)

# Link libraries
# set(LIBRARIES "glfw;Vulkan::Vulkan")
target_link_libraries(Frisbee_Engine PRIVATE imgui ${LIBRARIES})

# Set C++ standard
if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET Frisbee_Engine PROPERTY CXX_STANDARD 20)
endif()

# Set to WIN32 Executable
if(WIN32)
    set_property(TARGET Frisbee_Engine PROPERTY WIN32_EXECUTABLE TRUE)
endif()