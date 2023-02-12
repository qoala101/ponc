# Enable solution folders in Visual Studio and Folders in Xcode
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Macro that will configure an example application
macro(add_example_executable name)
  set(_Example_Sources
    ${ARGN}
  )

  # source_group("" FILES ${_Example_Sources})
  source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${_Example_Sources})

  file(GLOB _Example_CommonResources CONFIGURE_DEPENDS "${IMGUI_NODE_EDITOR_ROOT_DIR}/examples/data/*")
  file(GLOB _Example_Resources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/data/*")

  # message(FATAL_ERROR "_Example_Resources = ${_Example_Resources}")
  set(_Example_Type)

  if(WIN32)
    set(_Example_Type WIN32)

    set(ApplicationIcon ${IMGUI_NODE_EDITOR_ROOT_DIR}/examples/Application/Support/Icon.ico)
    file(TO_NATIVE_PATH "${ApplicationIcon}" ApplicationIcon)
    string(REPLACE "\\" "\\\\" ApplicationIcon "${ApplicationIcon}")
    configure_file(
      ${IMGUI_NODE_EDITOR_ROOT_DIR}/examples/Application/Support/Resource.rc.in
      ${CMAKE_CURRENT_BINARY_DIR}/Resource.rc
    )
    source_group(TREE "${IMGUI_NODE_EDITOR_ROOT_DIR}/examples" FILES ${_Example_CommonResources})
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${_Example_Resources})
    list(APPEND _Example_Resources
      ${CMAKE_CURRENT_BINARY_DIR}/Resource.rc
      ${_Example_CommonResources}
    )
    source_group("resources" FILES ${CMAKE_CURRENT_BINARY_DIR}/Resource.rc)
  elseif(APPLE)
    set(_Example_Type MACOSX_BUNDLE)

    set_source_files_properties(${_Example_Resources} ${_Example_CommonResources} PROPERTIES
      MACOSX_PACKAGE_LOCATION "Resources/data"
    )
    set(_Example_Icon "${IMGUI_NODE_EDITOR_ROOT_DIR}/examples/application/support/Icon.icns")
    list(APPEND _Example_Resources ${_Example_Icon})
    set_source_files_properties(${_Example_Icon} PROPERTIES
      MACOSX_PACKAGE_LOCATION "Resources"
    )
  endif()

  add_executable(${name} ${_Example_Type} ${_Example_Sources} ${_Example_Resources} ${_Example_CommonResources})

  target_link_libraries(${name} PRIVATE
    thirdparty::imgui
    thirdparty::imgui_node_editor
    thirdparty::imgui-filebrowser
    thirdparty::application
  )

  set(_ExampleBinDir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

  set_target_properties(${name} PROPERTIES
    FOLDER "examples"
    RUNTIME_OUTPUT_DIRECTORY "${_ExampleBinDir}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${_ExampleBinDir}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${_ExampleBinDir}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${_ExampleBinDir}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${_ExampleBinDir}"
    DEBUG_POSTFIX _d
    RELWITHDEBINGO_POSTFIX _rd
    MINSIZEREL_POSTFIX _r
    VS_DEBUGGER_WORKING_DIRECTORY ${_ExampleBinDir}
    MACOSX_BUNDLE_INFO_PLIST "${IMGUI_NODE_EDITOR_ROOT_DIR}/examples/application/support/Info.plist.in"
    MACOSX_BUNDLE_BUNDLE_NAME "${PACKAGE_NAME}"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.sandbox.collisions"
    MACOSX_BUNDLE_LONG_VERSION_STRING "${PACKAGE_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}"
    MACOSX_BUNDLE_ICON_FILE Icon.icns
  )

  add_custom_command(
    TARGET ${name}
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ARGS ${_ExampleBinDir}/data
  )

  set(_ResourceRoot ${CMAKE_CURRENT_SOURCE_DIR})

  foreach(_Resource ROOT "${IMGUI_NODE_EDITOR_ROOT_DIR}/examples/data" ${_Example_CommonResources} ROOT "${CMAKE_CURRENT_SOURCE_DIR}/data" ${_Example_Resources})
    if(_Resource STREQUAL ROOT)
      set(_ResourceRoot FALSE)
      continue()
    elseif(NOT _ResourceRoot)
      set(_ResourceRoot ${_Resource})
      continue()
    endif()

    if("${_Resource}" MATCHES "\.DS_Store$")
      list(REMOVE_ITEM _Example_Resources ${_Resource})
      list(REMOVE_ITEM _Example_CommonResources ${_Resource})
      continue()
    endif()

    file(RELATIVE_PATH _RelResource ${_ResourceRoot} ${_Resource})

    add_custom_command(
      TARGET ${name}
      PRE_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ARGS ${_Resource} ${_ExampleBinDir}/data/${_RelResource}
    )
  endforeach()
endmacro()

add_example_executable(esc
  core/flow/core_flow_calculator.cc

  core/core_app.cc
  core/core_diagram.cc
  core/core_i_family.cc
  core/core_i_node.cc
  core/core_id_generator.cc

  cpp/cpp_assert.cc
  cpp/cpp_scope_function.cc

  draw/pin/draw_empty_pin_drawer.cc
  draw/pin/draw_flow_input_pin_drawer.cc
  draw/pin/draw_flow_output_pin_drawer.cc

  draw/draw_constants.cc
  draw/draw_families_view.cc
  draw/draw_flow_tree_view.cc
  draw/draw_i_drawable.cc
  draw/draw_i_file_dialog.cc
  draw/draw_i_pin_drawer.cc
  draw/draw_main_window.cc
  draw/draw_menu_bar.cc
  draw/draw_open_file_dialog.cc
  draw/draw_save_as_file_dialog.cc

  json/json_diagram_serializer.cc
  json/json_i_family_parser.cc
  json/json_i_family_writer.cc
  json/json_i_node_parser.cc
  json/json_i_node_writer.cc
  json/json_link_serializer.cc

  impl/impl_attenuator_node.cc
  impl/impl_client_node.cc
  impl/impl_coupler_node.cc
  impl/impl_hub_node.cc
  impl/impl_input_node.cc
  impl/impl_splitter_node.cc

  esc_app.cc
  esc_editor_context_handle.cc
  esc_node_drawer.cc
  esc_state.cc
  esc_textures_handle.cc
  esc_types.cc

  main.cc
)

target_include_directories(esc
  PUBLIC ${PROJECT_SOURCE_DIR}/include
  PUBLIC ${PROJECT_SOURCE_DIR}/include/core
  PUBLIC ${PROJECT_SOURCE_DIR}/include/core/flow
  PUBLIC ${PROJECT_SOURCE_DIR}/include/cpp
  PUBLIC ${PROJECT_SOURCE_DIR}/include/draw
  PUBLIC ${PROJECT_SOURCE_DIR}/include/draw/pin
  PUBLIC ${PROJECT_SOURCE_DIR}/include/impl
  PUBLIC ${PROJECT_SOURCE_DIR}/include/json
)