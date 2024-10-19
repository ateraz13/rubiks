cmake_minimum_required(VERSION 3.6)

project(rubiks)

find_package(glm CONFIG REQUIRED)
find_package(GLEW REQUIRED)

add_subdirectory(extern/glfw-3.4)

add_executable(rubiks main.cxx gfx.cxx geom.cxx)
target_include_directories(rubiks ${GLEW_INCLUDE_DIRS})
target_link_libraries(rubiks PRIVATE glm::glm ${GLEW_LIBRARIES} glfw)
