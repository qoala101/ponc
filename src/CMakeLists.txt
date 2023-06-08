set(EXECUTABLE_PROPERTIES)

if(WIN32)
  set(EXECUTABLE_PROPERTIES WIN32)
endif()

add_executable(ponc
  ${EXECUTABLE_PROPERTIES}

  app/family_group/app_attenuator_family_group.cc
  app/family_group/app_client_family_group.cc
  app/family_group/app_coupler_family_group.cc
  app/family_group/app_input_family_group.cc
  app/family_group/app_splitter_family_group.cc

  app/app_app.cc
  app/app_impl.cc

  calc/calc_calculation_task.cc
  calc/calc_calculator.cc
  calc/calc_resolution.cc

  core/core_diagram.cc
  core/core_free_pin_family_group.cc
  core/core_i_family_group.cc
  core/core_i_family.cc
  core/core_i_node.cc
  core/core_id_generator.cc
  core/core_id_ptr.cc
  core/core_link.cc
  core/core_pin.cc
  core/core_project.cc
  core/core_settings.cc
  core/core_tags.cc

  coreui/event/coreui_event_loop.cc
  coreui/event/coreui_event.cc

  coreui/traits/coreui_empy_pin_traits.cc
  coreui/traits/coreui_float_pin_traits.cc
  coreui/traits/coreui_flow_pin_traits.cc
  coreui/traits/coreui_i_family_traits.cc
  coreui/traits/coreui_i_node_traits.cc
  coreui/traits/coreui_i_pin_traits.cc

  coreui/coreui_calculator.cc
  coreui/coreui_cloner.cc
  coreui/coreui_diagram.cc
  coreui/coreui_family.cc
  coreui/coreui_linker.cc
  coreui/coreui_log.cc
  coreui/coreui_native_facade.cc
  coreui/coreui_node_mover.cc
  coreui/coreui_node.cc
  coreui/coreui_project.cc

  cpp/cpp_scope_function.cc

  draw/diagram/popup/draw_connect_node_popup.cc
  draw/diagram/popup/draw_create_node_popup.cc
  draw/diagram/popup/draw_family_groups_menu.cc
  draw/diagram/popup/draw_i_popup.cc
  draw/diagram/popup/draw_link_popup.cc
  draw/diagram/popup/draw_node_popup.cc

  draw/diagram/draw_colored_text.cc
  draw/diagram/draw_diagram_editor.cc
  draw/diagram/draw_flow_icon.cc
  draw/diagram/draw_item_deleter.cc
  draw/diagram/draw_link.cc
  draw/diagram/draw_linker.cc
  draw/diagram/draw_node.cc
  draw/diagram/draw_tooltip.cc

  draw/dialog/draw_about_dialog.cc
  draw/dialog/draw_i_file_dialog.cc
  draw/dialog/draw_open_file_dialog.cc
  draw/dialog/draw_question_dialog.cc
  draw/dialog/draw_save_as_file_dialog.cc

  draw/view/draw_calculator_view.cc
  draw/view/draw_diagrams_view.cc
  draw/view/draw_disable_if.cc
  draw/view/draw_flow_tree_view.cc
  draw/view/draw_i_view.cc
  draw/view/draw_log_view.cc
  draw/view/draw_node_view.cc
  draw/view/draw_nodes_view.cc
  draw/view/draw_settings_table_row.cc
  draw/view/draw_settings_view.cc
  draw/view/draw_tree_node.cc

  draw/draw_main_menu_bar.cc
  draw/draw_main_window.cc
  draw/draw_recent_log.cc
  draw/draw_string_buffer.cc

  flow/flow_algorithms.cc
  flow/flow_node_flow.cc
  flow/flow_tree_traversal.cc

  json/json_diagram_serializer.cc
  json/json_i_family_parser.cc
  json/json_i_family_writer.cc
  json/json_i_node_parser.cc
  json/json_i_node_writer.cc
  json/json_link_serializer.cc
  json/json_project_serializer.cc
  json/json_settings_serializer.cc
  json/json_versifier.cc

  style/style_update_styles.cc
  style/style_utils.cc

  main.cc
)

target_include_directories(ponc
  PRIVATE
  ${PROJECT_SOURCE_DIR}/include/app
  ${PROJECT_SOURCE_DIR}/include/app/family_group
  ${PROJECT_SOURCE_DIR}/include/calc
  ${PROJECT_SOURCE_DIR}/include/core
  ${PROJECT_SOURCE_DIR}/include/coreui
  ${PROJECT_SOURCE_DIR}/include/coreui/event
  ${PROJECT_SOURCE_DIR}/include/coreui/traits
  ${PROJECT_SOURCE_DIR}/include/cpp
  ${PROJECT_SOURCE_DIR}/include/draw
  ${PROJECT_SOURCE_DIR}/include/draw/diagram
  ${PROJECT_SOURCE_DIR}/include/draw/diagram/popup
  ${PROJECT_SOURCE_DIR}/include/draw/dialog
  ${PROJECT_SOURCE_DIR}/include/draw/view
  ${PROJECT_SOURCE_DIR}/include/flow
  ${PROJECT_SOURCE_DIR}/include/json
  ${PROJECT_SOURCE_DIR}/include/style
)

target_link_libraries(ponc
  PRIVATE
  thirdparty::application
  thirdparty::imgui
  thirdparty::imgui_node_editor
  thirdparty::imgui-filebrowser
)

set_target_properties(ponc PROPERTIES
  DEBUG_POSTFIX _debug
)

add_custom_command(TARGET ponc POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory
  $<TARGET_FILE_DIR:ponc>/data

  COMMAND ${CMAKE_COMMAND} -E copy
  $<TARGET_PROPERTY:thirdparty::application,SOURCE_DIR>/../data/Cuprum-Bold.ttf
  $<TARGET_PROPERTY:thirdparty::application,SOURCE_DIR>/../data/Cuprum-OFL.txt
  $<TARGET_PROPERTY:thirdparty::application,SOURCE_DIR>/../data/Play-Regular.ttf
  $<TARGET_PROPERTY:thirdparty::application,SOURCE_DIR>/../data/Play-OFL.txt

  # vh: Use to copy resources.
  # ${CMAKE_CURRENT_SOURCE_DIR}/resource/RESOURCE_FILE
  $<TARGET_FILE_DIR:ponc>/data
)

if(FAIL_ON_WARNINGS)
  target_compile_options(ponc PRIVATE -Werror)
endif()