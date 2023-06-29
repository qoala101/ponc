/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "core_diagram.h"

#include <imgui_node_editor.h>

#include <algorithm>
#include <memory>
#include <string>
#include <utility>

#include "core_i_node.h"
#include "core_link.h"
#include "cpp_assert.h"

namespace vh::ponc::core {
///
Diagram::Diagram(std::string name, std::vector<std::unique_ptr<INode>> nodes,
                 std::vector<Link> links, std::vector<Area> areas)
    : name_{std::move(name)},
      nodes_{std::move(nodes)},
      links_{std::move(links)},
      areas_{std::move(areas)} {}

///
auto Diagram::GetIds(Diagram& diagram) -> std::vector<IdPtr> {
  auto ids = std::vector<IdPtr>{};

  for (const auto& node : diagram.nodes_) {
    const auto node_ids = INode::GetIds(*node);
    ids.insert(ids.cend(), node_ids.cbegin(), node_ids.cend());
  }

  for (auto& link : diagram.links_) {
    const auto link_ids = Link::GetIds(link);
    ids.insert(ids.cend(), link_ids.cbegin(), link_ids.cend());
  }

  return ids;
}

///
auto Diagram::FindNode(const Diagram& diagram, ne::NodeId node_id) -> INode& {
  const auto node = std::find_if(
      diagram.nodes_.cbegin(), diagram.nodes_.cend(),
      [node_id](const auto& node) { return node->GetId() == node_id; });

  Expects(node != diagram.nodes_.cend());
  return **node;
}

///
auto Diagram::FindPinNode(const Diagram& diagram, ne::PinId pin_id) -> INode& {
  const auto pin_node =
      std::find_if(diagram.nodes_.cbegin(), diagram.nodes_.cend(),
                   [pin_id](const auto& node) {
                     const auto node_pins = INode::GetAllPins(*node);

                     return std::any_of(node_pins.cbegin(), node_pins.cend(),
                                        [pin_id](const auto& node_pin) {
                                          return node_pin.first == pin_id;
                                        });
                   });

  Expects(pin_node != diagram.nodes_.cend());
  return **pin_node;
}

///
auto Diagram::FindLink(Diagram& diagram, ne::LinkId link_id) -> Link& {
  const auto link =
      std::find_if(diagram.links_.begin(), diagram.links_.end(),
                   [link_id](const auto& link) { return link.id == link_id; });

  Expects(link != diagram.links_.cend());
  return *link;
}

///
auto Diagram::FindPinLink(const Diagram& diagram, ne::PinId pin_id)
    -> std::optional<const Link*> {
  // NOLINTNEXTLINE(*-const-cast)
  return FindPinLink(const_cast<Diagram&>(diagram), pin_id);
}

///
auto Diagram::FindPinLink(Diagram& diagram, ne::PinId pin_id)
    -> std::optional<Link*> {
  const auto pin_link = std::find_if(
      diagram.links_.begin(), diagram.links_.end(),
      [pin_id](const auto& link) { return Link::HasPin(link, pin_id); });

  if (pin_link == diagram.links_.cend()) {
    return std::nullopt;
  }

  return &*pin_link;
}

///
auto Diagram::HasLink(const Diagram& diagram, ne::PinId pin_id) -> bool {
  return FindPinLink(diagram, pin_id).has_value();
}

///
auto Diagram::FindArea(const Diagram& diagram, ne::NodeId node_id)
    -> const Area& {
  // NOLINTNEXTLINE(*-const-cast)
  return FindArea(const_cast<Diagram&>(diagram), node_id);
}

///
auto Diagram::FindArea(Diagram& diagram, ne::NodeId node_id) -> Area& {
  const auto area =
      std::find_if(diagram.areas_.begin(), diagram.areas_.end(),
                   [node_id](const auto& area) { return area.id == node_id; });

  Expects(area != diagram.areas_.cend());
  return *area;
}

///
auto Diagram::GetName() const -> const std::string& { return name_; }

///
void Diagram::SetName(std::string name) { name_ = std::move(name); }

///
auto Diagram::GetNodes() const -> const std::vector<std::unique_ptr<INode>>& {
  return nodes_;
}

///
auto Diagram::EmplaceNode(std::unique_ptr<INode> node) -> INode& {
  return *nodes_.emplace_back(std::move(node));
}

///
void Diagram::DeleteNode(ne::NodeId node_id) {
  std::erase_if(
      nodes_, [node_id](const auto& node) { return node->GetId() == node_id; });
}

///
auto Diagram::GetLinks() const -> const std::vector<Link>& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Diagram*>(this)->GetLinksImpl();
}

///
auto Diagram::EmplaceLink(const Link& link) -> Link& {
  return links_.emplace_back(link);
}

///
void Diagram::DeleteLink(ne::LinkId link_id) {
  std::erase_if(links_,
                [link_id](const auto& link) { return link.id == link_id; });
}

///
auto Diagram::GetLinksImpl() -> std::vector<Link>& { return links_; }

///
auto Diagram::GetAreas() const -> const std::vector<Area>& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Diagram*>(this)->GetAreas();
}

///
auto Diagram::GetAreas() -> std::vector<Area>& { return areas_; }

///
auto Diagram::EmplaceArea(const Area& area) -> Area& {
  return areas_.emplace_back(area);
}

///
void Diagram::DeleteArea(ne::NodeId node_id) {
  std::erase_if(areas_,
                [node_id](const auto& area) { return area.id == node_id; });
}
}  // namespace vh::ponc::core