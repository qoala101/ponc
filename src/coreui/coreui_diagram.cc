
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
#include "core_id_generator.h"
#include "core_link.h"
#include "core_project.h"
#include "core_settings.h"
#include "coreui_family.h"
#include "coreui_i_family_traits.h"
#include "coreui_i_node_traits.h"
#include "coreui_i_pin_traits.h"
#include "coreui_link.h"
#include "coreui_linker.h"
#include "coreui_pin.h"
#include "coreui_project.h"
#include "cpp_assert.h"
#include "cpp_safe_ptr.h"
#include "cpp_share.h"
#include "flow_algorithms.h"
#include "flow_node_flow.h"
#include "flow_tree.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::coreui {
///
Diagram::Diagram(cpp::SafePtr<Project> parent_project,
                 cpp::SafePtr<core::Diagram> diagram)
    : parent_project_{std::move(parent_project)},
      diagram_{std::move(diagram)},
      linker_{safe_owner_.MakeSafe(this)} {}

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
auto Diagram::GetLinker() const -> const Linker& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Diagram*>(this)->GetLinker();
}

///
auto Diagram::GetLinker() -> Linker& { return linker_; }

///
auto Diagram::GetFamilyGroups() const -> const std::vector<FamilyGroup>& {
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
void Diagram::AddNode(std::unique_ptr<core::INode> node) const {
  parent_project_->GetEventLoop().PostEvent(
      [diagram = diagram_, node = cpp::Share(std::move(node))]() {
        diagram->EmplaceNode(std::move(*node));
      });
}

///
auto Diagram::GetFreePinFamily(ne::PinKind pin_kind) const -> auto& {
  const auto& families = parent_project_->GetProject().GetFamilies();

  const auto family = std::find_if(
      families.begin(), families.end(), [pin_kind](const auto& family) {
        if (family->GetType() != core::FamilyType::kFreePin) {
          return false;
        }

        const auto node = family->CreateNode();
        return core::INode::FindFirstPinOfKind(*node, pin_kind).has_value();
      });

  Expects(family != families.end());
  return **family;
}

///
void Diagram::DeleteNode(ne::NodeId node_id) const {
  const auto& node = core::Diagram::FindNode(*diagram_, node_id);

  if (const auto input_pin = node.GetInputPinId()) {
    const auto& free_pin_family = GetFreePinFamily(ne::PinKind::Input);

    MoveConnectedLinkToNewFreePin(*input_pin, ne::PinKind::Input,
                                  free_pin_family);
  }

  const auto& free_pin_family = GetFreePinFamily(ne::PinKind::Output);

  for (const auto ouput_pin : node.GetOutputPinIds()) {
    MoveConnectedLinkToNewFreePin(ouput_pin, ne::PinKind::Output,
                                  free_pin_family);
  }

  parent_project_->GetEventLoop().PostEvent(
      [diagram = diagram_, node_id]() { diagram->DeleteNode(node_id); });
}

///
void Diagram::DeleteNodeWithLinks(ne::NodeId node_id) const {
  const auto& node = core::Diagram::FindNode(*diagram_, node_id);

  for (const auto& pin : core::INode::GetAllPins(node)) {
    if (const auto link = core::Diagram::FindPinLink(*diagram_, pin.first)) {
      DeleteLink((*link)->id);
    }
  }

  parent_project_->GetEventLoop().PostEvent(
      [diagram = diagram_, node_id]() { diagram->DeleteNode(node_id); });
}

///
auto Diagram::GetLinks() const -> const std::vector<Link>& { return links_; }

///
void Diagram::CreateLink(ne::PinId start_pin_id, ne::PinId end_pin_id) const {
  const auto link_id =
      parent_project_->GetProject().GetIdGenerator().Generate<ne::LinkId>();

  parent_project_->GetEventLoop().PostEvent(
      [diagram = diagram_, link = core::Link{.id = link_id,
                                             .start_pin_id = start_pin_id,
                                             .end_pin_id = end_pin_id}]() {
        diagram->EmplaceLink(link);
      });
}

///
void Diagram::DeleteLink(ne::LinkId link_id) const {
  parent_project_->GetEventLoop().PostEvent(
      [diagram = diagram_, link_id]() { diagram->DeleteLink(link_id); });
}

///
auto Diagram::FamilyFrom(const core::IFamily& core_family) const {
  return Family{parent_project_, safe_owner_.MakeSafe(&core_family)};
}

///
void Diagram::UpdateFamilyGroups() {
  family_groups_.clear();

  for (const auto& core_family : parent_project_->GetProject().GetFamilies()) {
    auto family = FamilyFrom(*core_family);

    const auto group_label = core_family->CreateUiTraits()->GetGroupLabel();
    const auto group =
        std::find_if(family_groups_.begin(), family_groups_.end(),
                     [&group_label](const auto& group) {
                       return group.label == group_label;
                     });

    if (group != family_groups_.end()) {
      group->families.emplace_back(std::move(family));
      continue;
    }

    family_groups_.emplace_back(FamilyGroup{
        .label = group_label, .families = std::vector{std::move(family)}});
  }
}

///
auto Diagram::LinkFrom(const core::Link& core_link,
                       const flow::NodeFlows& node_flows) const {
  auto link = Link{
      .core_link = core_link,
      .thickness = 2,
  };

  const auto link_alpha =
      linker_.IsRepiningLink(link.core_link.id) ? 1.F / 2 : 1.F;

  const auto& settings = parent_project_->GetProject().GetSettings();

  if (!settings.color_flow) {
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

  link.color = core::Settings::GetFlowColor(settings, start_pin_flow);
  link.color.Value.w = link_alpha;

  return link;
}

///
void Diagram::UpdateLinks(const flow::NodeFlows& node_flows) {
  links_.clear();

  const auto& links = diagram_->GetLinks();
  links_.reserve(links.size());

  std::transform(links.begin(), links.end(), std::back_inserter(links_),
                 [this, &node_flows](const auto& link) {
                   return LinkFrom(link, node_flows);
                 });
}

///
auto Diagram::GetHeaderColor(const IHeaderTraits& header_traits,
                             const flow::NodeFlow& node_flow) const {
  const auto& settings = parent_project_->GetProject().GetSettings();

  if (!settings.color_flow) {
    return header_traits.GetColor();
  }

  if (const auto input_flow = node_flow.input_pin_flow) {
    return core::Settings::GetFlowColor(settings, input_flow->second);
  }

  if (node_flow.output_pin_flows.empty()) {
    return ImColor{1.F, 1.F, 1.F};
  }

  return core::Settings::GetFlowColor(
      settings, node_flow.output_pin_flows.begin()->second);
}

///
auto Diagram::PinFrom(const IPinTraits& pin_traits,
                      const flow::NodeFlow& node_flow) const {
  auto pin = Pin{.label = pin_traits.GetLabel()};

  const auto pin_value = pin_traits.GetValue();

  if (std::holds_alternative<float>(pin_value)) {
    pin.value = std::get<float>(pin_value);
  } else if (std::holds_alternative<float*>(pin_value)) {
    pin.value = std::get<float*>(pin_value);
  }

  const auto pin_type = pin_traits.GetPin();

  if (!std::holds_alternative<ne::PinId>(pin_type)) {
    return pin;
  }

  const auto pin_id = std::get<ne::PinId>(pin_type);
  const auto pin_flow = flow::NodeFlow::GetPinFlow(node_flow, pin_id);
  const auto& settings = parent_project_->GetProject().GetSettings();

  pin.flow_data = PinFlowData{
      .id = pin_id,
      .color = settings.color_flow
                   ? core::Settings::GetFlowColor(settings, pin_flow)
                   : ImColor{1.F, 1.F, 1.F},
      .filled = core::Diagram::FindPinLink(*diagram_, pin_id).has_value()};

  if (settings.color_flow && pin.label.has_value()) {
    pin.label->color = pin.flow_data->color;
  }

  if (!linker_.CanConnectToPin(pin_id)) {
    pin.flow_data->color.Value.w = 1.F / 4;
  }

  if (std::holds_alternative<PinFlow>(pin_value)) {
    pin.value = pin_flow;
  }

  return pin;
}

///
auto Diagram::NodeFrom(core::INode& core_node,
                       const flow::NodeFlow& node_flow) const {
  const auto node_traits = core_node.CreateUiTraits();
  auto node_data = NodeData{};

  if (const auto header_traits = node_traits->CreateHeaderTraits()) {
    const auto texture_file_path = (*header_traits)->GetTextureFilePath();

    node_data.header = Header{
        .label = node_traits->GetLabel(),
        .color = GetHeaderColor(**header_traits, node_flow),
        .texture =
            parent_project_->GetTexturesHandle().GetTexture(texture_file_path)};
  }

  for (const auto& pin_traits : node_traits->CreatePinTraits()) {
    auto& pins =
        (IPinTraits::GetPinKind(*pin_traits, core_node) == ne::PinKind::Input)
            ? node_data.input_pins
            : node_data.output_pins;

    pins.emplace_back(PinFrom(*pin_traits, node_flow));
  }

  return Node{safe_owner_.MakeSafe(&core_node), std::move(node_data)};
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

///
void Diagram::MoveConnectedLinkToNewFreePin(
    ne::PinId pin_id, ne::PinKind pin_kind,
    const core::IFamily& free_pin_family) const {
  const auto link = core::Diagram::FindPinLink(*diagram_, pin_id);

  if (!link.has_value()) {
    return;
  }

  auto free_pin = free_pin_family.CreateNode(
      parent_project_->GetProject().GetIdGenerator());

  core::Link::GetPinOfKind(**link, pin_kind) =
      core::INode::GetFirstPinOfKind(*free_pin, pin_kind);

  AddNode(std::move(free_pin));
}
}  // namespace esc::coreui