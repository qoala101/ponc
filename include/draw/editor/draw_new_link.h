/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_DRAW_NEW_LINK_H_
#define VH_DRAW_NEW_LINK_H_

#include <imgui_node_editor.h>

#include <optional>
#include <string>

#include "app_state.h"
#include "core_diagram.h"

namespace ne = ax::NodeEditor;

namespace esc::draw {
// ---
class NewLink {
 public:
  // ---
  void Draw(const AppState &app_state);
  // ---
  auto IsVisible [[nodiscard]] () const -> bool;
  // ---
  auto CanConnectToPin
      [[nodiscard]] (const AppState &app_state, ne::PinId pin_id) const -> bool;
  // ---
  auto GetDraggedFromPin [[nodiscard]] () const -> ne::PinId;

 private:
  // ---
  auto GetCanConnectToPinReason
      [[nodiscard]] (const core::Diagram &diagram, ne::PinId pin_id) const
      -> std::pair<bool, std::string>;

  // ---
  void AcceptNewLink(const AppState &app_state, std::string_view tooltip);

  // ---
  std::optional<ne::PinId> dragged_from_pin_{};
  // ---
  std::optional<ne::PinId> hovering_over_pin_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_NEW_LINK_H_
