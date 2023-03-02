#ifndef VH_COREUI_NODE_H_
#define VH_COREUI_NODE_H_

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
#include "coreui_link_creation.h"
#include "coreui_pin.h"
#include "coreui_texture.h"
#include "imgui.h"
#include "imgui_internal.h"

namespace esc::coreui {
///
struct Header {
  ///
  std::string label{};
  ///
  ImColor color{};
  ///
  Texture texture{};
};

struct Node {
  ///
  static auto FindPin(const Node &node, ne::PinId pin_id) -> const Pin &;

  ///
  ne::NodeId id{};
  ///
  std::optional<Header> header{};
  ///
  std::vector<Pin> input_pins{};
  ///
  std::vector<Pin> output_pins{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_NODE_H_
