#ifndef VH_DRAW_CREATION_H_
#define VH_DRAW_CREATION_H_

#include <imgui_node_editor.h>

#include <optional>
#include <string>

#include "draw_creation_popup.h"
#include "frame_node.h"
#include "imgui.h"

namespace esc::draw {

class Creation {
 public:
  using SignalCreateCurrentLink = std::function<void()>;
  using SignalCreateConnectedNode =
      std::function<void(const ImVec2 &new_node_pos, ne::PinId connect_to_pin)>;

  void Draw(coreui::Creation &creation,
            const SignalCreateCurrentLink &signal_create_current_link,
            const SignalCreateConnectedNode &signal_create_connected_node);

 private:
  std::optional<ne::PinId> dragged_from_pin_{};
  std::optional<ne::PinId> hovering_over_pin_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_CREATION_H_
