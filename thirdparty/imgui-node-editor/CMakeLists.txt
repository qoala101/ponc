include(FetchContent)

FetchContent_Declare(imgui-node-editor SYSTEM
  GIT_REPOSITORY https://github.com/thedmd/imgui-node-editor
  GIT_TAG 2f99b2d613a400f6579762bd7e7c343a0d844158
)

FetchContent_MakeAvailable(imgui-node-editor)

list(APPEND CMAKE_MODULE_PATH ${imgui-node-editor_SOURCE_DIR}/misc/cmake-modules)
set(IMGUI_NODE_EDITOR_ROOT_DIR ${imgui-node-editor_SOURCE_DIR})

find_package(imgui REQUIRED)
find_package(imgui_node_editor REQUIRED)

add_subdirectory(
  ${imgui-node-editor_SOURCE_DIR}/examples/application
  ${imgui-node-editor_BINARY_DIR}/examples/application
)

add_library(thirdparty::application ALIAS application)
add_library(thirdparty::imgui ALIAS imgui)
add_library(thirdparty::imgui_node_editor ALIAS imgui_node_editor)

if(WIN32)
  target_compile_definitions(application
    PRIVATE IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
    PRIVATE IMGUI_IMPL_WIN32_DISABLE_LINKING_XINPUT
  )
endif()

mark_as_system_target(application)
mark_as_system_target(imgui)
mark_as_system_target(imgui_node_editor)

if(LINUX)
  mark_as_system_target(gl3w)
endif()