
#include "coreui_diagram.h"

#include <algorithm>
#include <array>
#include <iterator>
#include <utility>
#include <variant>

#include "core_diagram.h"
#include "core_i_node.h"
#include "coreui_i_node_traits.h"
#include "coreui_i_pin_traits.h"
#include "coreui_link.h"
#include "coreui_link_creation.h"
#include "coreui_pin.h"
#include "cpp_assert.h"
#include "flow_algorithms.h"
#include "flow_node_flow.h"
#include "flow_tree.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::coreui {
///
Diagram::Diagram(cpp::SafePointer<core::Diagram> diagram, Hooks hooks)
    : diagram_{std::move(diagram)},
      hooks_{std::move(hooks)},
      link_creation_{{.find_pin_node = [diagram = diagram_](
                                           auto pin_id) -> const core::INode& {
                        return core::Diagram::FindPinNode(*diagram, pin_id);
                      },
                      .find_pin_link =
                          [diagram = diagram_](auto pin_id) {
                            return core::Diagram::FindPinLink(*diagram, pin_id);
                          }}} {}

///
void Diagram::OnFrame() {
  const auto flow_tree = flow::BuildFlowTree(*diagram_);
  const auto node_flows = flow::CalculateNodeFlows(flow_tree);

  UpdateLinks(node_flows);
  UpdateNodes(node_flows);
}

///
auto Diagram::GetLinkCreation() -> LinkCreation& { return link_creation_; }

///
auto Diagram::GetNodes() const -> const std::vector<Node>& { return nodes_; }

///
auto Diagram::GetLinks() const -> const std::vector<Link>& { return links_; }

///
auto Diagram::GetFlowLinkAlpha(ne::LinkId link_id) const {
  if (link_creation_.IsCreatingLink() && link_creation_.IsRepinningLink() &&
      link_creation_.IsLinkBeingRepinned(link_id)) {
    return 1.F / 2;
  }

  return 1.F;
}

///
auto Diagram::FlowLinkFrom(const core::Link& core_link,
                           const flow::NodeFlows& node_flows) const {
  auto link = Link{
      .type = core_link,
      .thickness = 2.F,
  };

  const auto link_alpha = GetFlowLinkAlpha(core_link.id);

  if (!hooks_.is_color_flow()) {
    link.color = {1.F, 1.F, 1.F, link_alpha};
    return link;
  }

  const auto start_pin_node_id =
      core::Diagram::FindPinNode(*diagram_, core_link.start_pin_id)
          .GetId()
          .Get();

  Expects(node_flows.contains(start_pin_node_id));
  const auto& node_flow = node_flows.at(start_pin_node_id);

  const auto start_pin_flow =
      flow::NodeFlow::GetPinFlow(node_flow, core_link.start_pin_id);

  link.color = hooks_.get_flow_color(start_pin_flow);
  link.color.Value.w = link_alpha;

  return link;
}

///
auto Diagram::GetPinIconTipPos(ne::PinId pin_id) const {
  const auto& pin_core_node = core::Diagram::FindPinNode(*diagram_, pin_id);
  const auto pin_node_id = pin_core_node.GetId();

  const auto pin_node = std::find_if(
      nodes_.begin(), nodes_.end(),
      [pin_node_id](const auto& node) { return node.id == pin_node_id; });
  Expects(pin_node != nodes_.end());

  const auto& pin = Node::FindPin(*pin_node, pin_id);
  Expects(pin.icon.has_value());

  const auto pin_rect = pin.icon->GetRect();
  const auto pin_kind = core::INode::GetPinKind(pin_core_node, pin_id);

  return std::pair{
      pin_kind,
      (pin_kind == ne::PinKind::Input)
          ? ImVec2{pin_rect.Min.x, (pin_rect.Min.y + pin_rect.Max.y) / 2}
          : ImVec2{pin_rect.Max.x, (pin_rect.Min.y + pin_rect.Max.y) / 2}};
}

///
auto Diagram::GetRepinningLinkColor() const {
  if (!link_creation_.IsHoveringOverPin()) {
    return ImColor{1.F, 1.F, 1.F};
  }

  if (link_creation_.CanCreateLink()) {
    return ImColor{1.F / 2, 1.F, 1.F / 2};
  }

  return ImColor{1.F, 1.F / 2, 1.F / 2};
}

///
auto Diagram::GetRepinningLink() const -> std::optional<Link> {
  if (!link_creation_.IsCreatingLink() || !link_creation_.IsRepinningLink()) {
    return std::nullopt;
  }

  auto link = Link{.color = GetRepinningLinkColor(), .thickness = 4.F};

  const auto fixed_pin_id = link_creation_.GetFixedPinOfLinkBeingRepinned();
  const auto [pin_kind, tip_pos] = GetPinIconTipPos(fixed_pin_id);

  if (pin_kind == ne::PinKind::Input) {
    link.type = HandmadeLink{.end_pos = tip_pos};
  } else {
    link.type = HandmadeLink{.start_pos = tip_pos};
  }

  return link;
}

///
void Diagram::UpdateLinks(const flow::NodeFlows& node_flows) {
  links_.clear();

  const auto& links = diagram_->GetLinks();
  std::transform(links.begin(), links.end(), std::back_inserter(links_),
                 [this, &node_flows](const auto& link) {
                   return FlowLinkFrom(link, node_flows);
                 });

  if (const auto repinning_link = GetRepinningLink()) {
    links_.emplace_back(repinning_link);
  }
}

///
auto Diagram::GetHeaderColor(const IHeaderTraits& header_traits,
                             const flow::NodeFlow& node_flow) const {
  if (!hooks_.is_color_flow()) {
    return header_traits.GetColor();
  }

  if (const auto input_flow = node_flow.input_pin_flow) {
    return hooks_.get_flow_color(input_flow->second);
  }

  return ImColor{1.F, 1.F, 1.F};
}

///
auto Diagram::GetPinIconAlpha(ne::PinId pin_id) const {
  if (link_creation_.IsCreatingLink() &&
      !link_creation_.CanConnectToPin(pin_id)) {
    return 1.F / 4;
  }

  return 1.F;
}

///
auto Diagram::PinFrom(const IPinTraits& pin_traits,
                      const flow::NodeFlow& node_flow) const {
  auto pin =
      Pin{.value = pin_traits.GetValue(), .label = pin_traits.GetLabel()};

  const auto pin_type = pin_traits.GetPin();

  if (!std::holds_alternative<ne::PinId>(pin_type)) {
    return pin;
  }

  const auto pin_id = std::get<ne::PinId>(pin_type);
  const auto pin_flow = flow::NodeFlow::GetPinFlow(node_flow, pin_id);

  pin.icon = PinIcon{
      {.id = pin_id,
       .color = hooks_.is_color_flow() ? hooks_.get_flow_color(pin_flow)
                                       : ImColor{1.F, 1.F, 1.F},
       .filled = core::Diagram::FindPinLink(*diagram_, pin_id).has_value()}};

  if (std::holds_alternative<std::monostate>(pin.value)) {
    pin.value = pin_flow;
  }

  return pin;
}

///
auto Diagram::NodeFrom(const core::INode& core_node,
                       const flow::NodeFlow& node_flow) const {
  auto node = Node{.id = core_node.GetId().Get()};
  const auto node_traits = core_node.CreateUiTraits();

  if (const auto header_traits = node_traits->CreateHeaderTraits()) {
    node.header = Header{
        .label = node_traits->GetLabel(),
        .color = GetHeaderColor(**header_traits, node_flow),
        .texture = hooks_.get_texture((*header_traits)->GetTextureFilePath())};
  }

  for (const auto& pin_traits : node_traits->CreatePinTraits()) {
    auto& pins =
        (IPinTraits::GetPinKind(*pin_traits, core_node) == ne::PinKind::Input)
            ? node.input_pins
            : node.output_pins;

    pins.emplace_back(PinFrom(*pin_traits, node_flow));
  }

  return node;
}

///
void Diagram::UpdateNodes(const flow::NodeFlows& node_flows) {
  nodes_.clear();

  const auto& nodes = diagram_->GetNodes();
  std::transform(nodes.begin(), nodes.end(), std::back_inserter(nodes_),
                 [this, &node_flows](const auto& node) {
                   const auto node_id = node->GetId().Get();
                   Expects(node_flows.contains(node_id));
                   return NodeFrom(*node, node_flows.at(node_id));
                 });
}
}  // namespace esc::coreui