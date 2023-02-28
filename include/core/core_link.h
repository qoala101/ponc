#ifndef VH_CORE_LINK_H_
#define VH_CORE_LINK_H_

#include <imgui_node_editor.h>

namespace ne = ax::NodeEditor;

namespace esc::core {
///
struct Link {
  ///
  static auto HasPin(const Link &link, ne::PinId pin_id) -> bool;

  ///
  ne::LinkId id{};
  ///
  ne::PinId start_pin_id{};
  ///
  ne::PinId end_pin_id{};

 private:
  ///
  friend auto operator==(const Link &, const Link &) -> bool = default;
};
}  // namespace esc::core

#endif  // VH_CORE_LINK_H_
