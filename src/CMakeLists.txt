add_executable(esc
  main.cc
)

target_link_libraries(esc
  PRIVATE
  imgui
  imgui_node_editor
  application
)