cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(Splat)

find_package(glfw3 REQUIRED)
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(tinyobjloader REQUIRED)

set(LIBRARIES ${LIBRARIES} ${OPENGL_LIBRARIES} glfw ${GLEW_LIBRARIES} ${TINYOBJLOADER_LIBRARIES})

# generate compile_commands.json
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

add_subdirectory(src)
