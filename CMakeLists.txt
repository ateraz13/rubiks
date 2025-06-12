cmake_minimum_required(VERSION 3.10)

project(rubiks)

set(CXX_STANDARD 20)

add_subdirectory(extern/glfw-3.4)

find_package(glm CONFIG REQUIRED)

set(RUBIKS_SOURCE_FILES main.cxx gfx.cxx geom.cxx game.cxx utility.cxx gl.cxx shader.cxx gl_calls.cxx window.cxx keys.cxx console.cxx command.cxx app.cxx)

file(GLOB IMGUI_SOURCE_FILES extern/imgui/*.cpp)
set(IMGUI_SOURCE_FILES ${IMGUI_SOURCE_FILES} extern/imgui/backends/imgui_impl_glfw.cpp extern/imgui/backends/imgui_impl_opengl3.cpp)

if(PROVIDE_INSPECT_GL_DEBUG_INFO)
  add_compile_definitions(INSPECT_GL_DEBUG_INFO=1)
endif()

add_executable(gen_mesh gen_mesh.cxx shader_preproc.cxx)
add_executable(shader_preproc shader_preproc.cxx)

if(USE_GLAD)
  set(GLAD_ROOT_DIR ${PROJECT_SOURCE_DIR}/extern/glad)
  set(GLAD_INCLUDE_DIR ${GLAD_ROOT_DIR}/include)
  include_directories(${GLEW_INCLUDE_DIRS} ${GLFW3_INCLUDE_DIRS} ${GLAD_INCLUDE_DIR} extern/imgui extern/imgui/backends)
  add_compile_definitions(USE_GLAD=1)
  add_executable(rubiks ${RUBIKS_SOURCE_FILES} ${GLAD_ROOT_DIR}/src/gl.c ${IMGUI_SOURCE_FILES})
  target_link_libraries(rubiks PRIVATE glm::glm glfw ${OPENGL_opengl_LIBR})
else()
  find_package(GLEW 2.1.0 REQUIRED)
  include_directories(${GLEW_INCLUDE_DIRS} ${GLFW3_INCLUDE_DIRS})
  add_executable(rubiks ${RUBIKS_SOURCE_FILES} ${OPENGL_opengl_LIBRARY} ${IMGUI_SOURCE_FILES})
  target_link_libraries(rubiks PRIVATE glm::glm glfw GLEW::GLEW ${OPENGL_opengl_LIBRARY})
endif()

add_custom_command(
  OUTPUT
   ${PROJECT_SOURCE_DIR}/simple_vertex_shader_data.hxx
  DEPENDS shader_preproc
  COMMAND shader_preproc
   ${PROJECT_SOURCE_DIR}/shaders/simple.vertex.glsl ${PROJECT_SOURCE_DIR}/simple_vertex_shader_data.hxx
)

# add_custom_command(
#   OUTPUT
#    ${PROJECT_SOURCE_DIR}/generated_mesh.cxx
#    ${PROJECT_SOURCE_DIR}/generated_mesh.hxx
#   DEPENDS gen_mesh
#   COMMAND gen_mesh
#    ${PROJECT_SOURCE_DIR}/generated_mesh.cxx
#    ${PROJECT_SOURCE_DIR}/generated_mesh.hxx
# )

add_executable(test_mesh test_mesh.cxx generated_mesh.cxx)

add_compile_definitions("$<$<CONFIG:DEBUG>:DEBUG_BUILD=1>")

# add_executable(test test.cxx window.cxx)
# target_link_libraries(test PRIVATE glm::glm glfw GLEW::GLEW ${OPENGL_opengl_LIBRARY})

set_property(TARGET rubiks PROPERTY CXX_STANDARD 20)
