cmake_minimum_required(VERSION 3.6)

project(rubiks)

find_package(glm CONFIG REQUIRED)

add_executable(rubiks main.cxx gfx.cxx geom.cxx)
target_link_libraries(rubiks PRIVATE glm::glm)
