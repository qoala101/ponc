# vh: Supresses target warnings and makes includes appear in <> brackets.
function(mark_as_system_target target)
  get_target_property(target_type ${target} TYPE)

  if(${target_type} STREQUAL "INTERFACE_LIBRARY")
    set(scope INTERFACE)
  else()
    set(scope PUBLIC)
  endif()

  get_target_property(include_directories ${target} INTERFACE_INCLUDE_DIRECTORIES)
  target_include_directories(${target} SYSTEM ${scope} ${include_directories})
endfunction(mark_as_system_target)

add_subdirectory(imgui-filebrowser)
add_subdirectory(imgui-node-editor)