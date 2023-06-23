cmake_minimum_required(VERSION 3.24)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
  "ponc"
  VERSION 0.5.2
  LANGUAGES CXX
)

option(FAIL_ON_WARNINGS "Whether to treat compilation warnings as errors.")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

if(LINUX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

  add_compile_options(
    -Wall
    -Wextra
    -pedantic
    -Wno-missing-braces
  )

  find_program(CPPCHECK cppcheck)

  if(CPPCHECK)
    set(CMAKE_CXX_CPPCHECK ${CPPCHECK})
    list(APPEND CMAKE_CXX_CPPCHECK
      "--enable=all"
      "--inconclusive"
      "--inline-suppr"
      "--quiet"
      "--suppress=*:${FETCHCONTENT_BASE_DIR}*"
      "--suppressions-list=${CMAKE_SOURCE_DIR}/.cppcheck"
    )

    if(FAIL_ON_WARNINGS)
      list(APPEND CMAKE_CXX_CPPCHECK
        "--error-exitcode=1"
      )
    endif()
  endif()
endif()

add_subdirectory(src)
add_subdirectory(thirdparty)