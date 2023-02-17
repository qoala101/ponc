#include "draw_open_file_dialog.h"

#include "app_events.h"
#include "app_state.h"

namespace esc::draw {
void DrawOpenFileDialog(State &state) {
  state.draw_state->open_file_dialog.Display();

  if (!state.draw_state->open_file_dialog.HasSelected()) {
    return;
  }

  state.event_queue->PostEvent(event::OpenDiagramFromFile{
      .file_path = state.draw_state->open_file_dialog.GetSelected().string()});
  state.draw_state->open_file_dialog.ClearSelected();
}
}  // namespace esc::draw