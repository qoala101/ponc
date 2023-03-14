#ifndef VH_COREUI_NODE_H_
#define VH_COREUI_NODE_H_

#include <imgui.h>
#include <imgui_internal.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "core_i_node.h"
#include "coreui_pin.h"
#include "coreui_texture.h"
#include "cpp_safe_ptr.h"
#include "imgui_node_editor.h"

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

///
struct NodeData {
  ///
  bool update_pos{};
  ///
  std::optional<Header> header{};
  ///
  std::vector<Pin> input_pins{};
  ///
  std::vector<Pin> output_pins{};
};

///
class Node {
 public:
  ///
  explicit Node(cpp::SafePtr<core::INode> node, NodeData data);

  ///
  auto GetNode() const -> core::INode &;
  ///
  auto GetData() const -> const NodeData &;
  ///
  auto GetSize() const -> const ImVec2 &;
  ///
  void SetSize(const ImVec2 &size);
  ///
  auto GetPinTipPos(ne::PinId pin_id) const -> ImVec2;
  ///
  void SetPinTipPos(ne::PinId pin_id, const ImVec2 &pos);

 private:
  ///
  cpp::SafePtr<core::INode> node_;
  ///
  NodeData data_{};
  ///
  ImVec2 size_{};
  ///
  std::unordered_map<uintptr_t, ImVec2> pin_tip_poses_{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_NODE_H_
