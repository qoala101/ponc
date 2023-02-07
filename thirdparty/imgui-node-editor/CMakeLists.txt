include(FetchContent)

FetchContent_Declare(imgui-node-editor SYSTEM
  GIT_REPOSITORY https://github.com/thedmd/imgui-node-editor
  GIT_TAG 2f99b2d613a400f6579762bd7e7c343a0d844158
)

FetchContent_MakeAvailable(imgui-node-editor)

set(IMGUI_NODE_EDITOR_ROOT_DIR ${imgui-node-editor_SOURCE_DIR})
list(APPEND CMAKE_MODULE_PATH ${IMGUI_NODE_EDITOR_ROOT_DIR}/misc/cmake-modules)

find_package(imgui REQUIRED)
find_package(imgui_node_editor REQUIRED)

add_subdirectory(
  ${imgui-node-editor_SOURCE_DIR}/examples/application
  ${imgui-node-editor_BINARY_DIR}/examples/application
)

add_library(thirdparty::imgui ALIAS imgui)
add_library(thirdparty::imgui_node_editor ALIAS imgui_node_editor)
add_library(thirdparty::application ALIAS application)