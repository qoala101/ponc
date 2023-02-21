/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "core_diagram.h"

#include <imgui_node_editor.h>
#include <imgui_node_editor_internal.h>

#include <algorithm>
#include <memory>
#include <ranges>

#include "core_group.h"
#include "core_i_node.h"
#include "core_link.h"
#include "cpp_assert.h"

namespace esc::core {
// ---
Diagram::Diagram(std::vector<std::shared_ptr<INode>> nodes,
                 std::vector<Link> links, std::vector<Group> groups)
    : nodes_{std::move(nodes)},
      links_{std::move(links)},
      groups_{std::move(groups)} {}

// ---
auto Diagram::GetNodes() const -> const std::vector<std::shared_ptr<INode>>& {
  return nodes_;
}

// ---
auto Diagram::EmplaceNode(std::shared_ptr<INode> node)
    -> const std::shared_ptr<INode>& {
  return nodes_.emplace_back(std::move(node));
}

// ---
void Diagram::EraseNode(ne::NodeId node_id) {
  const auto found_node = std::ranges::find_if(
      nodes_, [node_id](const auto& node) { return node->GetId() == node_id; });

  Expects(found_node != nodes_.end());
  nodes_.erase(found_node);
}

// ---
auto Diagram::GetLinks() const -> const std::vector<Link>& { return links_; }

// ---
auto Diagram::EmplaceLink(const Link& link) -> Link& {
  return links_.emplace_back(link);
}

// ---
void Diagram::EraseLink(ne::LinkId link_id) {
  const auto found_link = std::ranges::find_if(
      links_, [link_id](const auto& link) { return link.id == link_id; });

  Expects(found_link != links_.end());
  links_.erase(found_link);
}

// ---
auto Diagram::GetGroups() const -> const std::vector<Group>& { return groups_; }

// ---
auto Diagram::EmplaceGroup(Group group) -> Group& {
  return groups_.emplace_back(std::move(group));
}

// ---
void Diagram::EraseGroup(const Group& group) {
  const auto found_group = std::ranges::find_if(
      groups_,
      [&group](const auto& other_group) { return &other_group == &group; });

  Expects(found_group != groups_.end());
  groups_.erase(found_group);
}

// ---
auto FindPinNode(const Diagram& diagram, ne::PinId pin_id)
    -> const std::shared_ptr<INode>& {
  for (const auto& node : diagram.GetNodes()) {
    for (const auto node_pin_id : GetAllPinIds(*node)) {
      if (node_pin_id == pin_id) {
        return node;
      }
    }
  }

  Expects(false);
}

// ---
auto FindPinLink(const Diagram& diagram, ne::PinId pin_id)
    -> std::optional<const Link*> {
  for (const auto& link : diagram.GetLinks()) {
    if ((link.start_pin_id == pin_id) || (link.end_pin_id == pin_id)) {
      return &link;
    }
  }

  return std::nullopt;
}

// // ---
// auto FindNode(const Diagram& diagram, ne::NodeId node_id)
//     -> const std::shared_ptr<INode>& {
//   for (const auto& family : diagram.GetFamilies()) {
//     if (const auto* node = family->FindNode(node_id)) {
//       return *node;
//     }
//   }

//   Expects(false);
// }

// // ---
// void EraseNode(Diagram& diagram, ne::NodeId node_id) {
//   for (const auto& family : diagram.GetFamilies()) {
//     if (const auto* node = family->FindNode(node_id)) {
//       family->EraseNode(node_id);
//       return;
//     }
//   }
// }

// auto Diagram::FindLink(ne::LinkId id) -> Link& {
//   for (auto& link : links_) {
//     if (link.id == id) {
//       return link;
//     }
//   }

//   Expects(false);
// }

// auto Diagram::FindLinkFromPin(ne::PinId pin_id) -> const Link* {
//   for (const auto& link : links_) {
//     if ((link.start_pin_id == pin_id) || (link.end_pin_id == pin_id)) {
//       return &link;
//     }
//   }

//   return nullptr;
// }

auto GetSelectedNodeIds() -> std::vector<ne::NodeId> {
  const auto num_selected_objects = ne::GetSelectedObjectCount();

  auto selected_ids = std::vector<ne::NodeId>{};
  selected_ids.resize(num_selected_objects);

  const auto num_selected_nodes = ne::GetSelectedNodes(
      selected_ids.data(), static_cast<int>(selected_ids.size()));
  selected_ids.resize(num_selected_nodes);

  return selected_ids;
}

auto GetSelectedLinkIds() -> std::vector<ne::LinkId> {
  const auto num_selected_objects = ne::GetSelectedObjectCount();

  auto selected_ids = std::vector<ne::LinkId>{};
  selected_ids.resize(num_selected_objects);

  const auto num_selected_links = ne::GetSelectedLinks(
      selected_ids.data(), static_cast<int>(selected_ids.size()));
  selected_ids.resize(num_selected_links);

  return selected_ids;
}
}  // namespace esc::core