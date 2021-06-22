cmake_minimum_required(VERSION 3.10)

project(rs
  VERSION 1.0
  DESCRIPTION "Reed-Solomon error correction"
  LANGUAGES C
)

include(CTest)

set(CMAKE_BUILD_TYPE "Release")

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

link_directories($ENV{HOME}/lib)

add_executable(rs
  rs.c
)
target_include_directories(rs PUBLIC
  $ENV{HOME}/include
)
target_compile_definitions(rs PUBLIC
  _FORTIFY_SOURCE=2
)
target_compile_options(rs PUBLIC
  -Wall
  -Wextra
  -Werror
  -fstack-protector-all
)
target_link_libraries(rs
  correct
)

add_test(test_encode
  rs "hello world")
add_test(test_decode
  rs "bw1l0#q0r1v"
  1D84DF22B39A872B7B088DC0F76DA827CF719837F6AD4490E7E61CBD8AAC8705)
