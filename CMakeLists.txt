cmake_minimum_required(VERSION 3.15)

project(glim
  VERSION 0.1.0
  DESCRIPTION "The Glim Programming Language"
  LANGUAGES C)

set(CMAKE_C_STANDARD 11)

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

add_executable("${PROJECT_NAME}"
  "src/analysis.c"
  "src/error.c"
  "src/lexer.c"
  "src/main.c"
  "src/memory.c"
  "src/parser.c"
  "src/utils.c"
)
