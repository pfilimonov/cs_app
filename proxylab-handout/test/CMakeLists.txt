cmake_minimum_required(VERSION 3.23)

project(proxylabtests)

set(CMAKE_BUILD_TYPE Debug)

add_executable(testrunner)

target_sources(testrunner
  PRIVATE
  tests.c
  ../csapp.c
  ../parse.c

  PRIVATE
    FILE_SET HEADERS
    BASE_DIRS
      ..
    FILES
      ../csapp.h
      ../parse.h
)
