# Executable and Libraries
file(GLOB sources _source/*.cpp _source/physics/*.cpp)
add_executable(Frisbee_Engine ${sources})
target_include_directories(Frisbee_Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/_include)

# Internal Projects Linking )
target_link_libraries(Frisbee_Engine PRIVATE Renderer Pins)

# GLM
target_include_directories(Renderer PUBLIC ${CMAKE_SOURCE_DIR}/Projects/_include/glm)

# SHADERS
file(GLOB_RECURSE SHADER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert" "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag")

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
add_dependencies(Frisbee_Engine ShadersTarget)

# Set C++ standard
if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET Frisbee_Engine PROPERTY CXX_STANDARD 20)
endif()

# Set to WIN32 Executable
if(WIN32)
    set_property(TARGET Frisbee_Engine PROPERTY WIN32_EXECUTABLE TRUE)
endif()