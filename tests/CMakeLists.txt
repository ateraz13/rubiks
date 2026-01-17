include_directories(${GTEST_INCLUDE_DIRS})

set(SHADER_PARSER_TEST_DATA_HEADER ${PROJECT_SOURCE_DIR}/tests/file_includes/shader_parser_test_data.hxx)
add_custom_command(
  OUTPUT ${SHADER_PARSER_TEST_DATA_HEADER}
  COMMAND bin2c
  ARGS "${PROJECT_SOURCE_DIR}/tests/files/example_shader1.glsl" example_shader1 -o ${SHADER_PARSER_TEST_DATA_HEADER}
  DEPENDS bin2c "${PROJECT_SOURCE_DIR}/tests/files/example_shader1.glsl"
)

add_executable(shader_parser_test shader_parser_test.cxx ${SHADER_PARSER_TEST_DATA_HEADER})
target_include_directories(shader_parser_test PRIVATE ${PROJECT_SOURCE_DIR}/tests/)
target_link_libraries(shader_parser_test shader_parser)
target_include_directories(shader_parser_test PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(shader_parser_test gtest gtest_main pthread)

add_test(NAME ShaderParserTest COMMAND shader_parser_test)
