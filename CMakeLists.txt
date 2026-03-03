# ════════════════════════════════════════════════════════════════
# Project Configuration
# ════════════════════════════════════════════════════════════════

cmake_minimum_required(VERSION 3.20)

project(csapp VERSION 1.0.0 LANGUAGES C)

# ════════════════════════════════════════════════════════════════
# C Standard Settings
# ════════════════════════════════════════════════════════════════

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ════════════════════════════════════════════════════════════════
# Compiler Warnings
# ════════════════════════════════════════════════════════════════

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow           # Предупреждать о затенении переменных
        -Wstrict-prototypes  # Требовать явные прототипы функций
        -Wmissing-prototypes # Предупреждать об отсутствующих прототипах
    )
endif()

# ════════════════════════════════════════════════════════════════
# Sanitizers (Debug mode only)
# ════════════════════════════════════════════════════════════════

if(CMAKE_BUILD_TYPE MATCHES Debug AND NOT MSVC)
  add_compile_options(-fsanitize=address,undefined -g)
  add_link_options(-fsanitize=address,undefined)
endif()

# ════════════════════════════════════════════════════════════════
# Main Executable
# ════════════════════════════════════════════════════════════════

add_executable(csapp
    src/ch2/p258.c
)

target_include_directories(csapp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# csapp.h из книги требует эти библиотеки
target_link_libraries(csapp PRIVATE
    m        # libm — математические функции
    pthread  # POSIX threads
)

# ════════════════════════════════════════════════════════════════
# Tests (Optional, через Unity или простой main)
# ════════════════════════════════════════════════════════════════

option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
  enable_testing()

  add_executable(tests
        tests/test_main.c
    )

  target_include_directories(tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

  target_link_libraries(tests PRIVATE m pthread)

  add_test(NAME csapp_tests COMMAND tests)
endif()
