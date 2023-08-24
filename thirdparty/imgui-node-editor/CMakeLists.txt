include(FetchContent)

FetchContent_Declare(imgui-node-editor SYSTEM
  GIT_REPOSITORY https://github.com/qoala101/imgui-node-editor
  GIT_TAG 0ebed01c2c2e882a6727ffca8d51fb2f263f4eab
)

FetchContent_MakeAvailable(imgui-node-editor)

add_library(imgui_node_editor STATIC
  ${imgui-node-editor_SOURCE_DIR}/crude_json.cpp
  ${imgui-node-editor_SOURCE_DIR}/imgui_canvas.cpp
  ${imgui-node-editor_SOURCE_DIR}/imgui_canvas.cpp
  ${imgui-node-editor_SOURCE_DIR}/imgui_node_editor_api.cpp
  ${imgui-node-editor_SOURCE_DIR}/imgui_node_editor.cpp
)
add_library(thirdparty::imgui_node_editor ALIAS imgui_node_editor)

target_include_directories(imgui_node_editor
  PUBLIC
  ${imgui-node-editor_SOURCE_DIR}
)

target_compile_definitions(imgui_node_editor
  PUBLIC
  IMGUI_DEFINE_MATH_OPERATORS
)

target_link_libraries(imgui_node_editor
  PUBLIC
  thirdparty::imgui
)