include(FetchContent)

FetchContent_Declare(imgui_node_editor SYSTEM
  GIT_REPOSITORY https://github.com/thedmd/imgui-node-editor
  GIT_TAG 2f99b2d613a400f6579762bd7e7c343a0d844158
)

FetchContent_MakeAvailable(imgui_node_editor)

add_subdirectory(${imgui_node_editor_SOURCE_DIR}/examples ${imgui_node_editor_BINARY_DIR}/examples)