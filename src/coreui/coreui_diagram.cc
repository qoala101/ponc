
#include "coreui_diagram.h"

#include <algorithm>
#include <array>
#include <iterator>
#include <memory>
#include <utility>
#include <variant>
#include <vector>

#include "core_diagram.h"
#include "core_i_family.h"
#include "core_i_node.h"
#include "core_link.h"
#include "coreui_family.h"
#include "coreui_i_family_traits.h"
#include "coreui_i_node_traits.h"
#include "coreui_i_pin_traits.h"
#include "coreui_link.h"
#include "coreui_link_creation.h"
#include "coreui_pin.h"
#include "cpp_assert.h"
#include "cpp_safe_ptr.h"
#include "flow_algorithms.h"
#include "flow_node_flow.h"
#include "flow_tree.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::coreui {
///
Diagram::Diagram(
    cpp::SafePtr<core::Diagram> diagram,
    cpp::SafePtr<const std::vector<std::unique_ptr<core::IFamily>>> families,
    cpp::SafePtr<core::IdGenerator> id_generator, Callbacks callbacks)
    : diagram_{std::move(diagram)},
      families_{std::move(families)},
      id_generator_{std::move(id_generator)},
      callbacks_{std::move(callbacks)},
      link_creation_{
          {.find_pin_node = [safe_this = safe_owner_.MakeSafe(this)](
                                auto pin_id) -> const core::INode& {
             return core::Diagram::FindPinNode(*safe_this->diagram_, pin_id);
           },
           .find_pin_link =
               [safe_this = safe_owner_.MakeSafe(this)](auto pin_id) {
                 return core::Diagram::FindPinLink(*safe_this->diagram_,
                                                   pin_id);
               },
           .create_link =
               [safe_this = safe_owner_.MakeSafe(this)](auto start_pin_id,
                                                        auto end_pin_id) {
                 safe_this->callbacks_.post_event([safe_this, start_pin_id,
                                                   end_pin_id]() {
                   safe_this->diagram_->EmplaceLink(core::Link{
                       .id = safe_this->id_generator_->Generate<ne::LinkId>(),
                       .start_pin_id = start_pin_id,
                       .end_pin_id = end_pin_id});
                 });
               },
           .delete_link =
               [safe_this = safe_owner_.MakeSafe(this)](auto link_id) {
                 safe_this->callbacks_.post_event([safe_this, link_id]() {
                   safe_this->diagram_->EraseLink(link_id);
                 });
               }}} {}

///
void Diagram::OnFrame() {
  UpdateFamilyGroups();

  const auto flow_tree = flow::BuildFlowTree(*diagram_, safe_owner_);
  const auto node_flows = flow::CalculateNodeFlows(flow_tree);

  UpdateLinks(node_flows);
  UpdateNodes(node_flows);
}

///
auto Diagram::GetDiagram() const -> core::Diagram& { return *diagram_; }

///
auto Diagram::GetLinkCreation() -> LinkCreation& { return link_creation_; }

///
auto Diagram::GetFamilyGroups() -> const std::vector<FamilyGroup>& {
  return family_groups_;
}

///
auto Diagram::GetNodes() const -> const std::vector<Node>& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Diagram*>(this)->GetNodes();
}

///
auto Diagram::GetNodes() -> std::vector<Node>& { return nodes_; }

///
auto Diagram::GetLinks() const -> const std::vector<Link>& { return links_; }

///
auto Diagram::FamilyFrom(const core::IFamily& core_family) const {
  return Family{
      safe_owner_.MakeSafe(&core_family),
      id_generator_,
      {.node_created =
           [safe_this = safe_owner_.MakeSafe(this)](auto node) {
             safe_this->callbacks_.post_event(
                 [safe_this,
                  node = std::make_shared<std::unique_ptr<core::INode>>(
                      std::move(node))]() {
                   safe_this->diagram_->EmplaceNode(std::move(*node));
                 });
           },
       .link_created =
           [safe_this = safe_owner_.MakeSafe(this)](const auto& link) {
             safe_this->callbacks_.post_event([safe_this, link]() {
               safe_this->diagram_->EmplaceLink(link);
             });
           }}};
}

///
void Diagram::UpdateFamilyGroups() {
  family_groups_.clear();

  for (const auto& core_family : *families_) {
    auto family = FamilyFrom(*core_family);

    const auto group_label = core_family->CreateUiTraits()->GetGroupLabel();
    const auto group =
        std::find_if(family_groups_.begin(), family_groups_.end(),
                     [&group_label](const auto& group) {
                       return group.label == group_label;
                     });

    if (group == family_groups_.end()) {
      family_groups_.emplace_back(FamilyGroup{
          .label = group_label, .families = std::vector{std::move(family)}});
      continue;
    }

    group->families.emplace_back(std::move(family));
  }
}

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
      .thickness = 2,
  };

  const auto link_alpha = GetFlowLinkAlpha(core_link.id);

  if (!callbacks_.is_color_flow()) {
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

  link.color = callbacks_.get_flow_color(start_pin_flow);
  link.color.Value.w = link_alpha;

  return link;
}

///
auto Diagram::GetPinIconTipPos(ne::PinId pin_id, ne::PinKind pin_kind) const {
  const auto& pin_core_node = core::Diagram::FindPinNode(*diagram_, pin_id);
  const auto pin_node_id = pin_core_node.GetId();

  const auto pin_node = std::find_if(
      nodes_.begin(), nodes_.end(),
      [pin_node_id](const auto& node) { return node.id == pin_node_id; });
  Expects(pin_node != nodes_.end());

  const auto& pin = Node::FindPin(*pin_node, pin_id);
  Expects(pin.icon.has_value());

  const auto pin_rect = pin.icon->GetRect();

  return ImVec2{
      (pin_kind == ne::PinKind::Input) ? pin_rect.Min.x : pin_rect.Max.x,
      (pin_rect.Min.y + pin_rect.Max.y) / 2};
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

  auto link = Link{.color = GetRepinningLinkColor(), .thickness = 4};

  const auto [fixed_pin_id, pin_kind] =
      link_creation_.GetCurrentLinkSourcePin();
  const auto tip_pos = GetPinIconTipPos(fixed_pin_id, pin_kind);

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
  links_.reserve(links.size());

  std::transform(links.begin(), links.end(), std::back_inserter(links_),
                 [this, &node_flows](const auto& link) {
                   return FlowLinkFrom(link, node_flows);
                 });

  if (const auto repinning_link = GetRepinningLink()) {
    links_.emplace_back(*repinning_link);
  }
}

///
auto Diagram::GetHeaderColor(const IHeaderTraits& header_traits,
                             const flow::NodeFlow& node_flow) const {
  if (!callbacks_.is_color_flow()) {
    return header_traits.GetColor();
  }

  if (const auto input_flow = node_flow.input_pin_flow) {
    return callbacks_.get_flow_color(input_flow->second);
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
       .color = callbacks_.is_color_flow() ? callbacks_.get_flow_color(pin_flow)
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
  auto node = Node{.id = core_node.GetId().Get(), .pos = core_node.GetPos()};
  const auto node_traits = core_node.CreateUiTraits();

  if (const auto header_traits = node_traits->CreateHeaderTraits()) {
    node.header = Header{.label = node_traits->GetLabel(),
                         .color = GetHeaderColor(**header_traits, node_flow),
                         .texture = callbacks_.get_texture(
                             (*header_traits)->GetTextureFilePath())};
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
  nodes_.reserve(nodes.size());

  std::transform(nodes.begin(), nodes.end(), std::back_inserter(nodes_),
                 [this, &node_flows](const auto& node) {
                   const auto node_id = node->GetId().Get();
                   Expects(node_flows.contains(node_id));
                   return NodeFrom(*node, node_flows.at(node_id));
                 });
}
}  // namespace esc::coreui