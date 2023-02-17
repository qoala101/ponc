#include "draw_save_as_file_dialog.h"

#include "app_events.h"

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
  state.draw_state->open_file_dialog.Display();

  if (!state.draw_state->open_file_dialog.HasSelected()) {
    return;
  }

  auto file_path = state.draw_state->open_file_dialog.GetSelected().string();

  if (const auto not_json_extension =
          !AsLowerCase(file_path).ends_with(".json")) {
    file_path += ".json";
  }

  state.event_queue->PostEvent(
      event::SaveDiagramToFile{.file_path = file_path});
  state.draw_state->open_file_dialog.ClearSelected();
}
}  // namespace esc::draw