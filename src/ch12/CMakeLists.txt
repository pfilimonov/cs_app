cmake_minimum_required(VERSION 3.23)

project(ch12)

add_executable(solution)

target_sources(solution
  PRIVATE
  p1223.c
  pool.c
  rio.c
  helpers.c
)
