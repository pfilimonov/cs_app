cmake_minimum_required(VERSION 3.23)

project(ch12)

set(CMAKE_BUILD_TYPE Debug)

add_executable(solution)

target_sources(solution
  PRIVATE
  p1237.c
  pool.c
  rio.c
  helpers.c
)
