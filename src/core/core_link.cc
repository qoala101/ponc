#include "core_link.h"

#include "imgui_node_editor.h"

namespace esc::core {
///
auto Link::HasPin(const Link &link, ne::PinId pin_id) -> bool {
  return (pin_id == link.start_pin_id) || (pin_id == link.end_pin_id);
}

///
auto Link::GetPinKind(const Link &link, ne::PinId pin_id) -> ne::PinKind {
  if (pin_id == link.start_pin_id) {
    return ne::PinKind::Output;
  }

  return ne::PinKind::Input;
}

///
auto Link::GetOtherPin(const Link &link, ne::PinId pin_id) -> ne::PinId {
  if (pin_id == link.start_pin_id) {
    return link.end_pin_id;
  }

  return link.start_pin_id;
}
}  // namespace esc::core