#ifndef VH_COREUI_NODES_H_
#define VH_COREUI_NODES_H_

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
#include "coreui_handmade_link.h"
#include "coreui_i_pin_traits.h"
#include "coreui_link_creation.h"
#include "coreui_texture.h"
#include "imgui.h"
#include "imgui_internal.h"

namespace esc::coreui {
struct NodeHeader {
  std::string label{};
  ImColor color{};
  Texture texture{};
};

struct Pin {
  std::optional<ne::PinId> id{};
  std::variant<std::monostate, float, float *> value{};
  std::string label{};
  ImColor color{};
  bool filled{};
};

class Node {
 public:
  ne::NodeId id{};
  std::optional<NodeHeader> header{};
  std::vector<Pin> input_pins{};
  std::vector<Pin> output_pins{};

  auto GetPinRect(ne::PinId pin_id) const -> ImRect;
  void SetPinRects(std::unordered_map<uintptr_t, ImRect> pin_rects);

 private:
  std::unordered_map<uintptr_t, ImRect> pin_rects_{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_NODES_H_
