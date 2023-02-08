cmake_minimum_required(VERSION 3.25)

# Generates compile_commands.json which is used by external tools to parse the project.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
  "esc"
  VERSION 0.2.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

add_compile_options(
  -Wall
  -Wextra
  -pedantic
  -Wno-missing-braces

  # Uncomment to treat warnings like errors. Not convenient for development.
  # -Werror
  # -pedantic-errors
)

add_subdirectory(src)
add_subdirectory(thirdparty)