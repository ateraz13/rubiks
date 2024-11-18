cmake_minimum_required(VERSION 3.6)

project(rubiks)

set(CXX_STANDARD 20)

find_package(glm CONFIG REQUIRED)
find_package(GLEW REQUIRED)

# set(GLAD_ROOT_DIR ${PROJECT_SOURCE_DIR}/extern/glad)
# set(GLAD_INCLUDE_DIR ${GLAD_ROOT_DIR}/include)

add_subdirectory(extern/glfw-3.4)

find_package(GLEW 2.0 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${GLEW_INCLUDE_DIRS} ${GLFW3_INCLUDE_DIRS} ${GLAD_INCLUDE_DIR})
# add_executable(rubiks main.cxx gfx.cxx geom.cxx game.cxx utility.cxx ${GLAD_ROOT_DIR}/src/gl.c)
add_executable(rubiks main.cxx gfx.cxx geom.cxx game.cxx utility.cxx gl.c)

set_property(TARGET rubiks PROPERTY CXX_STANDARD 20)
target_link_libraries(rubiks PRIVATE glm::glm glfw GLEW::GLEW GL)
