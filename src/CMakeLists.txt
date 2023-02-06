add_executable(esc
  drawing.cpp
  esc_builders.cc
  esc_example.cc
  esc_types.cc
  main.cc
  widgets.cpp
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