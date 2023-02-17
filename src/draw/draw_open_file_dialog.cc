#include "draw_open_file_dialog.h"

#include "esc_state.h"

namespace esc::draw {
void DrawOpenFileDialog(State &state) {
  state.DRAW_.open_file_dialog.Display();

  if (!state.DRAW_.open_file_dialog.HasSelected()) {
    return;
  }

  state.PostEvent(
      [file_path = state.DRAW_.open_file_dialog.GetSelected().string()](
          auto &state) { State::OpenDiagramFromFile(state, file_path); });

  state.DRAW_.open_file_dialog.ClearSelected();
}
}  // namespace esc::draw