# Executable and Libraries
file(GLOB sources _src/*.cpp)
add_library(Renderer ${sources})
target_include_directories(Renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# GLM
target_include_directories(Renderer PUBLIC ${CMAKE_SOURCE_DIR}/Projects/_include/glm)

# TinyObjLoad
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor/tinyobjloader)

# GLFW
set(GLFW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw-3.3.9)
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
set(IMGUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui)
file(GLOB IMGUI_SOURCES ${IMGUI_INCLUDE_DIR}/*.cpp ${IMGUI_INCLUDE_DIR}/*.h) 
add_library("imgui" STATIC ${IMGUI_SOURCES})
target_include_directories("imgui" PUBLIC ${IMGUI_INCLUDE_DIR})
target_link_libraries("imgui" PRIVATE ${LIBRARIES})

# ImGuizmo
set(IMGUIZMO_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imguizmo)
file(GLOB IMGUIZMO_SOURCES ${IMGUIZMO_INCLUDE_DIR}/*.cpp ${IMGUIZMO_INCLUDE_DIR}/*.h) 
add_library("imguizmo" STATIC ${IMGUIZMO_SOURCES})
target_include_directories("imguizmo" PUBLIC ${IMGUIZMO_INCLUDE_DIR})
target_link_libraries("imguizmo" PRIVATE imgui)

#VMA
target_include_directories(Renderer PUBLIC "vendor/vma")

# Link libraries
target_link_libraries(Renderer PUBLIC imgui imguizmo ${LIBRARIES})

# Set C++ standard
if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET Renderer PROPERTY CXX_STANDARD 20)
endif()