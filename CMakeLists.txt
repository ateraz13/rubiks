cmake_minimum_required(VERSION 3.6)

project(rubiks)

set(CXX_STANDARD 20)

add_subdirectory(extern/glfw-3.4)

find_package(glm CONFIG REQUIRED)
#set(OpenGL_GL_PREFERENCE GLVND)
#find_package(OpenGL REQUIRED)

set(RUBIKS_SOURCE_FILES main.cxx gfx.cxx geom.cxx game.cxx utility.cxx gl.cxx shader.cxx gl_calls.cxx window.cxx keys.cxx)

# add_definitions(USE_GLAD 1)
if(USE_GLAD)
  set(GLAD_ROOT_DIR ${PROJECT_SOURCE_DIR}/extern/glad)
  set(GLAD_INCLUDE_DIR ${GLAD_ROOT_DIR}/include)
  include_directories(${GLEW_INCLUDE_DIRS} ${GLFW3_INCLUDE_DIRS} ${GLAD_INCLUDE_DIR})
  add_compile_definitions(USE_GLAD=1)
  add_executable(rubiks ${RUBIKS_SOURCE_FILES} ${GLAD_ROOT_DIR}/src/gl.c)
  target_link_libraries(rubiks PRIVATE glm::glm glfw ${OPENGL_opengl_LIBRARY})
else()
  find_package(GLEW 2.1.0 REQUIRED)
  include_directories(${GLEW_INCLUDE_DIRS} ${GLFW3_INCLUDE_DIRS})
  add_executable(rubiks ${RUBIKS_SOURCE_FILES} ${OPENGL_opengl_LIBRARY})
  target_link_libraries(rubiks PRIVATE glm::glm glfw GLEW::GLEW ${OPENGL_opengl_LIBRARY})
endif()

add_compile_definitions("$<$<CONFIG:DEBUG>:DEBUG_BUILD=1>")

# add_executable(test test.cxx window.cxx)
# target_link_libraries(test PRIVATE glm::glm glfw GLEW::GLEW ${OPENGL_opengl_LIBRARY})

set_property(TARGET rubiks PROPERTY CXX_STANDARD 20)
