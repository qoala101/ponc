#include "core_diagram.h"

#include <crude_json.h>
#include <imgui_node_editor.h>
#include <imgui_node_editor_internal.h>
#include <sys/types.h>

#include <cstring>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "core_link.h"
#include "core_placeholder_family.h"
#include "cpp_assert.h"
#include "draw_i_node_drawer.h"

namespace esc::core {
Diagram::Diagram(std::vector<std::shared_ptr<IFamily>> families,
                 std::vector<Link> links)
    : families_{std::move(families)},
      links_{std::move(links)},
      placeholder_family_{std::make_shared<PlaceholderFamily>()} {
  families_.emplace_back(placeholder_family_);
}

auto Diagram::GetLinks() const -> const std::vector<Link>& { return links_; }

auto Diagram::GetPlaceholderFamily() const -> PlaceholderFamily& {
  return *placeholder_family_;
}

auto Diagram::FindNode(ne::NodeId id) -> INode& {
  for (const auto& family : families_) {
    for (const auto& node : family->GetNodes()) {
      if (node->GetId() == id) {
        return *node;
      }
    }
  }

  cpp::Expects(false);
}

auto Diagram::FindNodePTR(ne::NodeId id) -> const std::shared_ptr<INode>& {
  for (const auto& family : families_) {
    for (const auto& node : family->GetNodes()) {
      if (node->GetId() == id) {
        return node;
      }
    }
  }

  cpp::Expects(false);
}

auto Diagram::FindPin(ne::PinId id, const State& state)
    -> std::unique_ptr<draw::IPinDrawer> {
  return FindPinNode(id)->CreateDrawer(state)->CreatePinDrawer(id);
}

auto Diagram::FindPinNode(ne::PinId id) -> const std::shared_ptr<INode>& {
  for (const auto& family : families_) {
    for (const auto& node : family->GetNodes()) {
      for (const auto pin_id : node->GetPinIds()) {
        if (pin_id == id) {
          return node;
        }
      }
    }
  }

  cpp::Expects(false);
}

auto Diagram::FindLink(ne::LinkId id) -> Link& {
  for (auto& link : links_) {
    if (link.id == id) {
      return link;
    }
  }

  cpp::Expects(false);
}

auto Diagram::FindLinkFromPin(ne::PinId pin_id) -> const Link* {
  for (const auto& link : links_) {
    if ((link.start_pin_id == pin_id) || (link.end_pin_id == pin_id)) {
      return &link;
    }
  }

  return nullptr;
}

auto Diagram::EmplaceLink(const Link& link) -> Link& {
  return links_.emplace_back(link);
}

void Diagram::EraseLink(ne::LinkId linkId) {
  auto id = std::find_if(links_.begin(), links_.end(),
                         [linkId](auto& link) { return link.id == linkId; });
  if (id != links_.end()) links_.erase(id);
}

void Diagram::EraseNode(ne::NodeId id) {
  for (const auto& family : families_) {
    for (const auto& node : family->GetNodes()) {
      if (node->GetId() == id) {
        family->EraseNode(id);
        return;
      }
    }
  }
}

auto Diagram::GetSelectedNodeIds() -> std::vector<ne::NodeId> {
  const auto num_selected_objects = ne::GetSelectedObjectCount();

  auto selected_ids = std::vector<ne::NodeId>{};
  selected_ids.resize(num_selected_objects);

  const auto num_selected_nodes = ne::GetSelectedNodes(
      selected_ids.data(), static_cast<int>(selected_ids.size()));
  selected_ids.resize(num_selected_nodes);

  return selected_ids;
}

auto Diagram::GetSelectedLinkIds() -> std::vector<ne::LinkId> {
  const auto num_selected_objects = ne::GetSelectedObjectCount();

  auto selected_ids = std::vector<ne::LinkId>{};
  selected_ids.resize(num_selected_objects);

  const auto num_selected_links = ne::GetSelectedLinks(
      selected_ids.data(), static_cast<int>(selected_ids.size()));
  selected_ids.resize(num_selected_links);

  return selected_ids;
}

auto Diagram::GetFamilies() const
    -> const std::vector<std::shared_ptr<IFamily>>& {
  return families_;
}
}  // namespace esc::core