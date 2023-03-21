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
#include "coreui_flow_tree.h"
#include "coreui_pin.h"
#include "coreui_texture.h"
#include "cpp_safe_ptr.h"

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
struct FlowValue {
  ///
  float value{};
  ///
  ImColor color{};
};

///
struct NodeFlow {
  ///
  std::optional<FlowValue> input_flow{};
  ///
  std::vector<FlowValue> output_flows{};
};

///
struct NodeData {
  ///
  std::string label{};
  ///
  std::optional<Header> header{};
  ///
  std::vector<Pin> input_pins{};
  ///
  std::vector<Pin> output_pins{};
  ///
  NodeFlow flow{};
};

///
class Node {
 public:
  ///
  explicit Node(cpp::SafePtr<core::INode> node, NodeData data);

  ///
  auto GetNode() const -> core::INode &;
  ///
  auto GetTreeNode() const -> const TreeNode &;
  ///
  void SetTreeNode(cpp::SafePtr<const TreeNode> tree_node);
  ///
  auto GetData() const -> const NodeData &;

 private:
  ///
  cpp::SafePtr<core::INode> node_;
  ///
  std::optional<cpp::SafePtr<const TreeNode>> tree_node_{};
  ///
  NodeData data_{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_NODE_H_
