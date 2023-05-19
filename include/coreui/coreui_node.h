/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_COREUI_NODE_H_
#define VH_PONC_COREUI_NODE_H_

#include <imgui.h>

#include <optional>
#include <string>
#include <vector>

#include "core_i_node.h"
#include "coreui_flow_tree_node.h"
#include "coreui_pin.h"
#include "cpp_safe_ptr.h"

namespace vh::ponc::coreui {
///
struct Header {
  ///
  std::string label{};
  ///
  ImColor color{};
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
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_NODE_H_
