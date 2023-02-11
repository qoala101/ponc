#include "draw_save_as_file_dialog.h"

#include "draw_i_file_dialog.h"

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
        dialog.SetTitle("Save Diagram As JSON");
        return dialog;
      }()} {}

void SaveAsFileDialog::OnFileSelected(State& state,
                                      std::string file_path) const {
  if (const auto not_json_extension =
          !AsLowerCase(file_path).ends_with(".json")) {
    file_path += ".json";
  }

  state.SaveDiagramToFile(file_path);
}
}  // namespace esc::draw