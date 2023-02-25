#include "draw_save_as_file_dialog.h"

#include "frame_node.h"

namespace esc::draw {
namespace {
auto AsLowerCase(std::string text) {
  for (auto& character : text) {
    character = static_cast<char>(std::tolower(character));
  }

  return text;
}
}  // namespace

SaveAsFileDialog::SaveAsFileDialog()
    : IFileDialog{[]() {
        auto dialog =
            ImGui::FileBrowser{ImGuiFileBrowserFlags_EnterNewFilename |
                               ImGuiFileBrowserFlags_CreateNewDir |
                               ImGuiFileBrowserFlags_CloseOnEsc};
        dialog.SetTitle("Save Project As JSON");
        return dialog;
      }()} {}

void SaveAsFileDialog::OnFileSelected(frame::Frame& frame,
                                      std::string file_path) const {
  if (const auto not_json_extension =
          !AsLowerCase(file_path).ends_with(".json")) {
    file_path += ".json";
  }

  frame.SaveProjectToFile(std::move(file_path));
}
}  // namespace esc::draw