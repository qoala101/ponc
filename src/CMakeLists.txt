add_executable(esc
  esc_builders.cc
  esc_drawing.cc
  esc_example.cc
  esc_types.cc
  esc_widgets.cc
  main.cc
)

target_include_directories(esc
  PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(esc
  PRIVATE
  application
  imgui
  imgui_node_editor
)