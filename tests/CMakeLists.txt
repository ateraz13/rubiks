include_directories(${GTEST_INCLUDE_DIRS})

set(SHADER_PARSER_TEST_DATA_HEADER ${CMAKE_SOURCE_DIR}/tests//file_includes/shader_parser_test_data.hxx)
add_custom_command(
  OUTPUT ${SHADER_PARSER_TEST_DATA_HEADER}
  COMMAND bin2c ${CMAKE_SOURCE_DIR}/tests/files/example_shader1.glsl example_shader1 -o ${SHADER_PARSER_TEST_DATA_HEADER}
  DEPENDS files/example_shader1.glsl
)

include_directories(${CMAKE_SOURCE_DIR}/tests/)
add_executable(shader_parser_test shader_parser_test.cxx ${SHADER_PARSER_TEST_DATA_HEADER})
target_link_libraries(shader_parser_test shader_parser)
target_include_directories(shader_parser_test PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(shader_parser_test gtest gtest_main pthread)

add_test(NAME ShaderParserTest COMMAND shader_parser_test)
