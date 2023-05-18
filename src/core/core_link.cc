/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "core_link.h"

#include "imgui_node_editor.h"

namespace vh::ponc::core {
///
auto Link::GetIds(Link &link) -> std::vector<IdPtr> {
  return {&link.id, &link.start_pin_id, &link.end_pin_id};
}

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

///
auto Link::GetPinOfKind(const Link &link, ne::PinKind pin_kind) -> ne::PinId {
  // NOLINTNEXTLINE(*-const-cast)
  return GetPinOfKind(const_cast<Link &>(link), pin_kind);
}

///
auto Link::GetPinOfKind(Link &link, ne::PinKind pin_kind) -> ne::PinId & {
  if (pin_kind == ne::PinKind::Input) {
    return link.end_pin_id;
  }

  return link.start_pin_id;
}
}  // namespace vh::ponc::core