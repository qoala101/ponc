#ifndef VH_COREUI_LINK_H_
#define VH_COREUI_LINK_H_

#include <imgui_node_editor.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "core_diagram.h"
#include "core_i_family.h"
#include "core_i_node.h"
#include "core_id_generator.h"
#include "core_link.h"
#include "core_project.h"
#include "coreui_i_pin_traits.h"
#include "coreui_texture.h"
#include "imgui.h"
#include "imgui_internal.h"

namespace esc::coreui {
///
struct MousePos {};

///
struct HandmadeLink {
  ///
  std::variant<ImVec2, MousePos> start_pos{};
  ///
  std::variant<ImVec2, MousePos> end_pos{};
};

///
struct Link {
  ///
  std::variant<core::Link, HandmadeLink> type{};
  ///
  ImColor color{};
  ///
  float thickness{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_LINK_H_
