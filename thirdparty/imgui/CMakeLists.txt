include(FetchContent)

FetchContent_Declare(imgui SYSTEM
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_TAG f7eea6387202f1174fdb7072311156d667a6826c
)

FetchContent_MakeAvailable(imgui)

set(BACKENDS ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)

if(LINUX)
  list(APPEND BACKENDS ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp)
else()
  list(APPEND BACKENDS ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp)
endif()

add_library(imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${BACKENDS}
)
add_library(thirdparty::imgui ALIAS imgui)

target_include_directories(imgui
  PUBLIC
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
)

mark_as_system_target(imgui)