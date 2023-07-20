/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "core_diagram.h"

#include <imgui_node_editor.h>

#include <algorithm>
#include <exception>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <variant>

#include "core_connection.h"
#include "core_i_node.h"
#include "core_link.h"
#include "cpp_assert.h"

namespace vh::ponc::core {
namespace {
///
auto HasDiagramNamed(std::string_view name,
                     const std::vector<Diagram>& diagrams) {
  return std::any_of(
      diagrams.cbegin(), diagrams.cend(),
      [name](const auto& diagram) { return diagram.GetName() == name; });
}

///
void RemoveIndexedPostfix(std::string& name, std::string_view postfix) {
  if (postfix.empty()) {
    return;
  }

  const auto postfix_pos = name.find_last_of(postfix);

  if (postfix_pos == std::string::npos) {
    return;
  }

  const auto after_postfix = name.substr(postfix_pos + 1);
  const auto before_postfix_pos = postfix_pos - postfix.size();

  if (after_postfix.empty()) {
    name = name.substr(0, before_postfix_pos);
    return;
  }

  try {
    const auto index = std::stoi(after_postfix);

    if (const auto same_postfix =
            after_postfix == (" " + std::to_string(index))) {
      name = name.substr(0, before_postfix_pos);
    }
  } catch (const std::exception&) {
  }
}
}  // namespace

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

  ids.reserve(ids.size() + diagram.areas_.size());
  std::transform(diagram.areas_.begin(), diagram.areas_.end(),
                 std::back_inserter(ids), [](auto& area) { return &area.id; });

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
auto Diagram::FindLink(const Diagram& diagram, ne::LinkId link_id)
    -> const Link& {
  // NOLINTNEXTLINE(*-const-cast)
  return FindLink(const_cast<Diagram&>(diagram), link_id);
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
auto Diagram::FindArea(const Diagram& diagram, AreaId area_id) -> const Area& {
  // NOLINTNEXTLINE(*-const-cast)
  return FindArea(const_cast<Diagram&>(diagram), area_id);
}

///
auto Diagram::FindArea(Diagram& diagram, AreaId area_id) -> Area& {
  const auto area =
      std::find_if(diagram.areas_.begin(), diagram.areas_.end(),
                   [area_id](const auto& area) { return area.id == area_id; });

  Expects(area != diagram.areas_.cend());
  return *area;
}

///
auto Diagram::MakeUniqueDiagramName(const std::vector<Diagram>& diagrams,
                                    std::string source_name,
                                    std::string_view postfix) -> std::string {
  if (!HasDiagramNamed(source_name, diagrams)) {
    return source_name;
  }

  RemoveIndexedPostfix(source_name, postfix);
  source_name += " ";

  if (!postfix.empty()) {
    source_name += postfix;

    if (!HasDiagramNamed(source_name, diagrams)) {
      return source_name;
    }

    source_name += " ";
  }

  auto index = 2;
  auto name = std::string{};
  auto name_is_unique = false;

  while (!name_is_unique) {
    name = source_name + std::to_string(index);
    name_is_unique = !HasDiagramNamed(name, diagrams);
    ++index;
  }

  return name;
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
void Diagram::DeleteArea(AreaId area_id) {
  std::erase_if(areas_,
                [area_id](const auto& area) { return area.id == area_id; });
}

///
void Diagram::OnConnectionDeleted(ConnectionId connection_id) {
  for (auto& link : links_) {
    if (std::holds_alternative<core::ConnectionId>(link.connection) &&
        (std::get<core::ConnectionId>(link.connection) == connection_id)) {
      link.connection = {};
    }
  }
}
}  // namespace vh::ponc::core