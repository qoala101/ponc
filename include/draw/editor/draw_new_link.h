/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_DRAW_NEW_LINK_H_
#define VH_DRAW_NEW_LINK_H_

#include <imgui_node_editor.h>

#include <optional>
#include <string>

#include "app_state.h"
#include "draw_new_link_popup.h"

namespace ne = ax::NodeEditor;

namespace esc::draw {
class NewLink {
 public:
  void Draw(const AppState &app_state);
  auto GetDraggedFromPin() const -> const std::optional<ne::PinId> &;
  auto GetHoveringOverPin() const -> const std::optional<ne::PinId> &;

 private:
  NewLinkPopup new_link_popup{};

  std::optional<ne::PinId> dragged_from_pin_{};
  std::optional<ne::PinId> hovering_over_pin_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_NEW_LINK_H_
