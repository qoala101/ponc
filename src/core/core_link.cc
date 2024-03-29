/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "core_link.h"

#include <imgui_node_editor.h>

#include <type_traits>
#include <variant>

#include "core_project.h"

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

///
auto Link::GetDrop(const Link &link, const Project &project) -> float {
  return std::visit(
      [&link, &project](const auto &v) {
        using V = std::remove_cvref_t<decltype(v)>;

        if constexpr (std::is_same_v<V, std::monostate>) {
          return 0.F;
        } else if constexpr (std::is_same_v<V, core::ConnectionId>) {
          const auto &connection = Project::FindConnection(project, v);
          return -link.length * connection.drop_per_length -
                 connection.drop_added;
        } else if constexpr (std::is_same_v<V, core::CustomConnection>) {
          return -link.length * v.drop_per_length - v.drop_added;
        }
      },
      link.connection);
}
}  // namespace vh::ponc::core