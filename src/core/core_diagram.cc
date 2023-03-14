#include "core_diagram.h"

#include <imgui_node_editor.h>
#include <imgui_node_editor_internal.h>

#include <algorithm>
#include <cstddef>
#include <memory>

#include "core_i_node.h"
#include "core_link.h"
#include "cpp_assert.h"

namespace esc::core {
///
Diagram::Diagram(std::vector<std::unique_ptr<INode>> nodes,
                 std::vector<Link> links)
    : nodes_{std::move(nodes)}, links_{std::move(links)} {}

///
auto Diagram::FindNode(const Diagram& diagram, ne::NodeId node_id)
    -> const INode& {
  const auto& nodes = diagram.GetNodes();
  const auto node = std::find_if(
      nodes.begin(), nodes.end(),
      [node_id](const auto& node) { return node->GetId() == node_id; });

  Expects(node != nodes.end());
  return **node;
}

///
auto Diagram::FindPinNode(const Diagram& diagram, ne::PinId pin_id)
    -> const INode& {
  const auto& nodes = diagram.GetNodes();
  const auto pin_node =
      std::find_if(nodes.begin(), nodes.end(), [pin_id](const auto& node) {
        const auto node_pins = INode::GetAllPins(*node);

        return std::any_of(node_pins.begin(), node_pins.end(),
                           [pin_id](const auto& node_pin) {
                             return node_pin.first == pin_id;
                           });
      });

  Expects(pin_node != nodes.end());
  return **pin_node;
}

///
auto Diagram::FindLink(Diagram& diagram, ne::LinkId link_id) -> Link& {
  auto& links = diagram.GetLinksImpl();
  const auto link =
      std::find_if(links.begin(), links.end(),
                   [link_id](const auto& link) { return link.id == link_id; });

  Expects(link != links.end());
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
  auto& links = diagram.GetLinksImpl();
  const auto pin_link = std::find_if(
      links.begin(), links.end(),
      [pin_id](const auto& link) { return Link::HasPin(link, pin_id); });

  if (pin_link == links.end()) {
    return std::nullopt;
  }

  return &*pin_link;
}

///
auto Diagram::GetNodes() const -> const std::vector<std::unique_ptr<INode>>& {
  return nodes_;
}

///
auto Diagram::EmplaceNode(std::unique_ptr<INode> node) -> INode& {
  const auto& new_node = nodes_.emplace_back(std::move(node));
  Ensures(!nodes_.empty());
  return *new_node;
}

///
void Diagram::DeleteNode(ne::NodeId node_id) {
  Expects(!nodes_.empty());

  const auto found_node = std::find_if(
      nodes_.begin(), nodes_.end(),
      [node_id](const auto& node) { return node->GetId() == node_id; });

  Expects(found_node != nodes_.end());
  nodes_.erase(found_node);
}

///
auto Diagram::GetLinks() const -> const std::vector<Link>& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Diagram*>(this)->GetLinksImpl();
}

///
auto Diagram::EmplaceLink(const Link& link) -> Link& {
  auto& new_link = links_.emplace_back(link);
  Ensures(!links_.empty());
  return new_link;
}

///
void Diagram::DeleteLink(ne::LinkId link_id) {
  Expects(!links_.empty());

  const auto found_link =
      std::find_if(links_.begin(), links_.end(),
                   [link_id](const auto& link) { return link.id == link_id; });

  Expects(found_link != links_.end());
  links_.erase(found_link);
}

///
auto Diagram::GetLinksImpl() -> std::vector<Link>& { return links_; }
}  // namespace esc::core