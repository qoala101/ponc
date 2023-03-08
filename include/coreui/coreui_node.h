#ifndef VH_COREUI_NODE_H_
#define VH_COREUI_NODE_H_

#include <imgui_node_editor.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "coreui_pin.h"
#include "coreui_texture.h"
#include "imgui.h"

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
  ne::NodeId id{};
  ///
  ImVec2 pos{};
  ///
  std::optional<Header> header{};
  ///
  std::vector<Pin> input_pins{};
  ///
  std::vector<Pin> output_pins{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_NODE_H_
