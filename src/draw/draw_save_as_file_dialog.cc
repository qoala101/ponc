#include "draw_save_as_file_dialog.h"

namespace esc::draw {
namespace {
auto AsLowerCase(std::string text) {
  for (auto& character : text) {
    character = static_cast<char>(std::tolower(character));
  }

  return text;
}
}  // namespace

void DrawSaveAsFileDialog(State& state) {
  state.DRAW_.open_file_dialog.Display();

  if (!state.DRAW_.open_file_dialog.HasSelected()) {
    return;
  }

  auto file_path = state.DRAW_.open_file_dialog.GetSelected().string();

  if (const auto not_json_extension =
          !AsLowerCase(file_path).ends_with(".json")) {
    file_path += ".json";
  }

  state.PostEvent([file_path = std::move(file_path)](auto& state) {
    State::SaveDiagramToFile(state, file_path);
  });

  state.DRAW_.open_file_dialog.ClearSelected();
}
}  // namespace esc::draw