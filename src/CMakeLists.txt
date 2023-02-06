add_executable(esc
  builders.cpp
  drawing.cpp
  example.cc
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