# Supresses target warnings and makes includes appear in <> brackets.
function(mark_as_system_target target)
  get_target_property(target_type ${target} TYPE)

  if(${target_type} STREQUAL "INTERFACE_LIBRARY")
    set(SCOPE INTERFACE)
  else()
    set(SCOPE PUBLIC)
  endif()

  get_target_property(INCLUDE_DIRECTORIES ${target} INTERFACE_INCLUDE_DIRECTORIES)

  target_include_directories(${target}
    SYSTEM ${SCOPE}
    ${INCLUDE_DIRECTORIES}
  )
endfunction(mark_as_system_target)

add_subdirectory(imgui-filebrowser)
add_subdirectory(imgui-node-editor)