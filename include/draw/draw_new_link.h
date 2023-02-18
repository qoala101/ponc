#ifndef VH_DRAW_NEW_LINK_H_
#define VH_DRAW_NEW_LINK_H_

#include <imgui_node_editor.h>

#include <iostream>
#include <optional>

#include "core_state.h"

namespace ne = ax::NodeEditor;

namespace esc::draw {
struct Rebind {
  ne::PinId fixed_pin{};
  ne::PinKind fixed_pin_kind{};
  ne::LinkId rebinding_link_id{};
  std::optional<ImVec2> fixed_pin_pos{};
};

struct NewLink {
  ne::PinId pin_dragged_from{};
  std::optional<ne::PinId> pin_hovered_over{};
  std::optional<Rebind> rebind{};
};

inline auto CanConnectFromPinToPin(core::CoreState &core_state,
                            std::optional<NewLink> &new_link,
                            ne::PinId start_pin, ne::PinId end_pin) -> bool {
  // if (start_pin == end_pin) {
  //   return false;
  // }

  // const auto rebind = new_link.has_value() && new_link->rebind.has_value();
  // const auto &families = core_state.diagram_.GetFamilies();
  // const auto &links = core_state.diagram_.GetLinks();

  // auto *start_node = static_cast<core::INode *>(nullptr);
  // auto *end_node = static_cast<core::INode *>(nullptr);

  // for (const auto &family : families) {
  //   for (const auto &node : family->GetNodes()) {
  //     for (const auto pin : node->GetPinIds()) {
  //       if (pin == start_pin) {
  //         start_node = &*node;
  //       } else if (pin == end_pin) {
  //         end_node = &*node;
  //       }
  //     }
  //   }
  // }

  // if (start_node == end_node) {
  //   std::cout << "r0\n";
  //   return false;
  // }

  // auto a = StateNoQueue{.core_state = &core_state, .draw_state = this};
  // const auto start_drawer =
  //     start_node->CreateDrawer(a)->CreatePinDrawer(start_pin);
  // const auto end_drawer = end_node->CreateDrawer(a)->CreatePinDrawer(end_pin);

  // if (rebind) {
  //   if (start_drawer->GetKind() != end_drawer->GetKind()) {
  //     std::cout << "r1\n";
  //     return false;
  //   }
  // } else {
  //   if (start_drawer->GetKind() == end_drawer->GetKind()) {
  //     std::cout << "r2\n";
  //     return false;
  //   }
  // }

  // auto *start_link = static_cast<const core::Link *>(nullptr);
  // auto *end_link = static_cast<const core::Link *>(nullptr);

  // for (const auto &link : links) {
  //   if (link.start_pin_id == start_pin || link.end_pin_id == start_pin) {
  //     start_link = &link;
  //   } else if (link.start_pin_id == end_pin || link.end_pin_id == end_pin) {
  //     end_link = &link;
  //   }
  // }

  // if (rebind) {
  //   std::cout << "rebind1\n";
  //   if (start_link != nullptr && end_link != nullptr) {
  //     std::cout << "rebind2\n";
  //     if (start_link != end_link) {
  //       std::cout << "rebind3\n";
  //       return false;
  //     }

  //     // return f
  //   }
  // } else {
  //   if (start_link != nullptr || end_link != nullptr) {
  //     std::cout << "r3\n";
  //     return false;
  //   }
  // }

  return true;
}
}  // namespace esc::draw

#endif  // VH_DRAW_NEW_LINK_H_
