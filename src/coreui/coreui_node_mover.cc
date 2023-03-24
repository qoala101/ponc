#include <algorithm>
#include <functional>
#include <iostream>
#include <numeric>

#include "core_diagram.h"
#include "cpp_assert.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "core_i_node.h"
#include "coreui_diagram.h"
#include "coreui_node_mover.h"
#include "flow_tree_traversal.h"
#include "imgui.h"

namespace vh::ponc::coreui {
namespace {
///
auto GetNodesPerLevel(const flow::TreeNode& tree_node,
                      const core::Diagram& diagram) {
  auto level = 0;
  auto nodes_per_level = std::vector<std::vector<core::INode*>>{};

  flow::TraverseDepthFirst(
      tree_node,
      [&diagram, &level, &nodes_per_level](const auto& tree_node) {
        auto& node = core::Diagram::FindNode(diagram, tree_node.node_id);

        if ((static_cast<int>(nodes_per_level.size()) - 1) < level) {
          nodes_per_level.emplace_back().emplace_back(&node);
        } else {
          nodes_per_level[level].emplace_back(&node);
        }

        ++level;
      },
      [&level](const auto&) { --level; });

  return nodes_per_level;
}
}  // namespace

///
NodeMover::NodeMover(cpp::SafePtr<Diagram> parent_diagram)
    : parent_diagram_{std::move(parent_diagram)} {}

///
void NodeMover::OnFrame() {
  MoveNewNodes();
  ApplyMoves();

  nodes_to_move_.clear();
  node_sizes_.clear();
  pin_poses_.clear();
}

///
void NodeMover::MoveNode(ne::NodeId node_id) {
  nodes_to_move_.insert(node_id.Get());
}

///
void NodeMover::MoveNodesTo(const std::vector<ne::NodeId>& node_ids,
                            ImVec2 pos) {
  const auto& diagram = parent_diagram_->GetDiagram();

  for (const auto node_id : node_ids) {
    auto& node = core::Diagram::FindNode(diagram, node_id);
    node.SetPos(pos);
    MoveNode(node_id);

    pos.y += GetNodeSize(node_id).y;
  }
}

///
void NodeMover::MakeTree(const flow::TreeNode& tree_node) {
  const auto nodes_per_level =
      GetNodesPerLevel(tree_node, parent_diagram_->GetDiagram());

  if (nodes_per_level.size() <= 1) {
    return;
  }

  Expects(!nodes_per_level.front().empty());

  const auto* root_node = nodes_per_level.front().front();
  const auto root_pos = root_node->GetPos();

  auto previous_level_rect = ImRect{{}, GetNodeSize(root_node->GetId())};
  previous_level_rect.Translate(root_pos);

  for (auto nodes = nodes_per_level.begin() + 1; nodes != nodes_per_level.end();
       ++nodes) {
    const auto level_height =
        std::accumulate(nodes->begin(), nodes->end(), 0.F,
                        [this](const auto height, const auto* node) {
                          return height + GetNodeSize(node->GetId()).y;
                        });

    const auto level_width = std::transform_reduce(
        nodes->begin(), nodes->end(), 0.F,
        [](const auto max_width, const auto node_width) {
          return std::max(max_width, node_width);
        },
        [this](const auto* node) { return GetNodeSize(node->GetId()).x; });

    auto level_rect = ImRect{{}, {level_width, level_height}};
    level_rect.Translate(
        {previous_level_rect.Max.x,
         previous_level_rect.GetCenter().y - level_rect.GetCenter().y});

    auto next_node_y = level_rect.Min.y;

    for (auto* node : *nodes) {
      node->SetPos({level_rect.Min.x, next_node_y});
      MoveNode(node->GetId());

      next_node_y += GetNodeSize(node->GetId()).y;
    }

    previous_level_rect = level_rect;
  }
}

///
void NodeMover::MovePinTo(ne::PinId pin_id, const ImVec2& pos) {
  const auto current_pin_pos = GetPinPos(pin_id);

  auto& node =
      core::Diagram::FindPinNode(parent_diagram_->GetDiagram(), pin_id);
  const auto matching_node_pos = node.GetPos() - current_pin_pos + pos;

  node.SetPos(matching_node_pos);
  MoveNode(node.GetId());
}

///
auto NodeMover::GetNodeSize(ne::NodeId node_id) const -> const ImVec2& {
  const auto node_id_value = node_id.Get();
  Expects(node_sizes_.contains(node_id_value));
  return node_sizes_.at(node_id_value);
}

///
void NodeMover::SetNodeSize(ne::NodeId node_id, const ImVec2& size) {
  node_sizes_.emplace(node_id, size);
}

///
auto NodeMover::GetPinPos(ne::PinId pin_id) const -> const ImVec2& {
  const auto pin_id_value = pin_id.Get();
  Expects(pin_poses_.contains(pin_id_value));
  return pin_poses_.at(pin_id_value);
}

///
void NodeMover::SetPinPos(ne::PinId pin_id, const ImVec2& pos) {
  pin_poses_.emplace(pin_id, pos);
}

///
void NodeMover::MoveNewNodes() {
  const auto& diagram = parent_diagram_->GetDiagram();

  for (const auto& node : diagram.GetNodes()) {
    const auto node_id = node->GetId();

    if (const auto node_is_new = !node_sizes_.contains(node_id.Get())) {
      MoveNode(node_id);
    }
  }
}

///
void NodeMover::ApplyMoves() const {
  const auto& diagram = parent_diagram_->GetDiagram();

  for (const auto node_id : nodes_to_move_) {
    ne::SetNodePosition(node_id,
                        core::Diagram::FindNode(diagram, node_id).GetPos());
  }
}
}  // namespace vh::ponc::coreui