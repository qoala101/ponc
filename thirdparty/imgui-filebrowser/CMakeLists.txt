include(FetchContent)

FetchContent_Declare(imgui-filebrowser SYSTEM
  GIT_REPOSITORY https://github.com/AirGuanZ/imgui-filebrowser
  GIT_TAG cfccc2aab651cb19cbc2c3ad36be78c36078ec76
)

FetchContent_MakeAvailable(imgui-filebrowser)

add_library(imgui-filebrowser INTERFACE)
add_library(thirdparty::imgui-filebrowser ALIAS imgui-filebrowser)

target_include_directories(imgui-filebrowser
  SYSTEM INTERFACE
  ${imgui-filebrowser_SOURCE_DIR}
)

mark_as_system_target(imgui-filebrowser)