#ifndef VH_FRAME_NODE_H_
#define VH_FRAME_NODE_H_

#include <imgui_node_editor.h>

#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "core_link.h"
#include "core_project.h"
#include "draw_texture.h"
#include "draw_widgets.h"
#include "imgui.h"

namespace ne = ax::NodeEditor;

namespace esc::frame {
struct Pin {
  std::optional<ne::PinId> id{};
  ImColor color{};
  bool filled{};
  std::string label{};
  std::variant<std::monostate, float, float *> value{};
  ImRect drawn_rect{};
};

struct NodeHeader {
  ImColor color{};
  std::string label{};
};

struct Node {
  ne::NodeId id{};
  std::optional<NodeHeader> header{};
  std::vector<Pin> input_pins{};
  std::vector<Pin> output_pins{};
};

struct Link {
  core::Link link{};
  ImColor color{};
  float thickness{};
};

struct Frame {
  Frame(const core::Project &project, const draw::Widgets &widgets);

  std::vector<Node> nodes{};
  std::vector<Link> links{};
};
}  // namespace esc::frame

#endif  // VH_FRAME_NODE_H_
