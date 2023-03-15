#include "coreui_linker.h"

#include <iostream>
#include <optional>
#include <vector>

#include "core_diagram.h"
#include "core_i_node.h"
#include "core_link.h"
#include "core_pin.h"
#include "coreui_diagram.h"
#include "cpp_assert.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::coreui {
///
Linker::Linker(cpp::SafePtr<Diagram> parent_diagram)
    : parent_diagram_{std::move(parent_diagram)} {}

///
auto Linker::GetCurrentLinkSourcePin() const -> auto& {
  Expects(linking_data_.has_value());

  if (const auto& repinning_data = linking_data_->repinning_data) {
    return repinning_data->fixed_pin_data;
  }

  return linking_data_->dragged_from_pin_data;
}

///
void Linker::SetPins(const std::optional<ne::PinId>& dragged_from_pin,
                     const std::optional<ne::PinId>& hovering_over_pin) {
  if (const auto creating_node = linking_data_.has_value() &&
                                 linking_data_->creating_data.has_value()) {
    return;
  }

  linking_data_.reset();

  if (!dragged_from_pin.has_value()) {
    return;
  }

  const auto& diagram = parent_diagram_->GetDiagram();
  const auto& dragged_from_node =
      core::Diagram::FindPinNode(diagram, *dragged_from_pin);

  linking_data_ = LinkingData{
      .dragged_from_pin_data = {
          .id = *dragged_from_pin,
          .kind = core::INode::GetPinKind(dragged_from_node, *dragged_from_pin),
          .node_id = dragged_from_node.GetId()}};

  const auto link_to_repin =
      core::Diagram::FindPinLink(diagram, *dragged_from_pin);

  if (link_to_repin.has_value()) {
    const auto fixed_pin =
        core::Link::GetOtherPin(**link_to_repin, *dragged_from_pin);

    linking_data_->repinning_data = RepinningData{
        .link_to_repin = (*link_to_repin)->id,
        .fixed_pin_data = {
            .id = fixed_pin,
            .kind = core::Link::GetPinKind(**link_to_repin, fixed_pin),
            .node_id = core::Diagram::FindPinNode(diagram, fixed_pin).GetId()}};

    UpdateManualLinks();
  }

  if (!hovering_over_pin.has_value()) {
    return;
  }

  const auto& hovering_over_pin_node =
      core::Diagram::FindPinNode(diagram, *hovering_over_pin);

  linking_data_->hovering_data = HoveringData{
      .hovering_over_pin = *hovering_over_pin,
      .hovering_over_pin_kind =
          core::INode::GetPinKind(hovering_over_pin_node, *hovering_over_pin),
      .reason = GetCanConnectToPinReason(*hovering_over_pin).second};
}

///
auto Linker::CanConnectToPin(ne::PinId pin_id) const -> bool {
  return GetCanConnectToPinReason(pin_id).first;
}

///
auto Linker::CanConnectToFamily(const core::IFamily& family) const -> bool {
  if (!linking_data_.has_value()) {
    return true;
  }

  const auto sample_node = family.CreateSampleNode();
  const auto& source_pin = GetCurrentLinkSourcePin();
  const auto target_kind = core::Pin::GetOppositeKind(source_pin.kind);

  return core::INode::FindFirstPinOfKind(*sample_node, target_kind).has_value();
}

///
auto Linker::GetCanCreateLinkReason() const -> std::pair<bool, std::string> {
  if (!linking_data_.has_value()) {
    return {true, {}};
  }

  const auto& hovering_data = linking_data_->hovering_data;

  if (!hovering_data.has_value()) {
    return {true, {}};
  }

  return GetCanConnectToPinReason(hovering_data->hovering_over_pin);
}

///
auto Linker::IsRepiningLink(ne::LinkId link_id) const -> bool {
  if (!linking_data_.has_value()) {
    return false;
  }

  const auto& repinning_data = linking_data_->repinning_data;

  if (!repinning_data.has_value()) {
    return false;
  }

  return repinning_data->link_to_repin == link_id;
}

///
void Linker::AcceptCreateLink() {
  Expects(linking_data_.has_value());
  const auto& hovering_data = linking_data_->hovering_data;
  Expects(hovering_data.has_value());

  CreateOrRepinCurrentLink(hovering_data->hovering_over_pin);
}

///
void Linker::StartCreatingNodeAt(const ImVec2& new_node_pos) {
  Expects(linking_data_.has_value());
  auto& creating_data = linking_data_->creating_data;
  Expects(!creating_data.has_value());

  creating_data = CreatingData{.new_node_pos = new_node_pos};
  UpdateManualLinks();

  const auto& source_pin = GetCurrentLinkSourcePin();

  if (source_pin.kind == ne::PinKind::Input) {
    return;
  }

  const auto& diagram = parent_diagram_->GetDiagram();
  const auto& source_node =
      core::Diagram::FindNode(diagram, source_pin.node_id);

  for (const auto pin_id : source_node.GetOutputPinIds()) {
    if (pin_id == source_pin.id) {
      continue;
    }

    if (core::Diagram::HasLink(diagram, pin_id)) {
      continue;
    }

    creating_data->source_node_empty_pins.emplace_back(pin_id);
  }
}

///
auto Linker::CanCreateNodesForAllPins() const -> bool {
  if (!linking_data_.has_value()) {
    return false;
  }

  const auto& creating_data = linking_data_->creating_data;
  return creating_data.has_value() &&
         !creating_data->source_node_empty_pins.empty();
}

///
void Linker::SetCreateNodesForAllPins(bool create_for_all_pins) {
  Expects(linking_data_.has_value());
  auto& creating_data = linking_data_->creating_data;
  Expects(creating_data.has_value());

  creating_data->create_for_all_pins = create_for_all_pins;
  UpdateManualLinks();
}

void Linker::AcceptCreateNode(const Family& family) {
  Expects(linking_data_.has_value());
  const auto& creating_data = linking_data_->creating_data;
  Expects(creating_data.has_value());

  const auto target_kind =
      core::Pin::GetOppositeKind(GetCurrentLinkSourcePin().kind);
  auto created_nodes = std::vector<ne::NodeId>{};

  if (creating_data->create_for_all_pins) {
    for (const auto empty_source_pin : creating_data->source_node_empty_pins) {
      auto new_node = family.CreateNode();
      new_node->SetPos(creating_data->new_node_pos);

      CreateLink(empty_source_pin,
                 core::INode::GetFirstPinOfKind(*new_node, target_kind));

      created_nodes.emplace_back(new_node->GetId());
      parent_diagram_->AddNode(std::move(new_node));
    }
  }

  auto new_node = family.CreateNode();
  new_node->SetPos(creating_data->new_node_pos);

  CreateOrRepinCurrentLink(
      core::INode::GetFirstPinOfKind(*new_node, target_kind));

  created_nodes.emplace_back(new_node->GetId());
  parent_diagram_->AddNode(std::move(new_node));

  // parent_diagram_->PlaceVertically();
}

///
void Linker::DiscardCreateNode() { linking_data_.reset(); }

///
auto Linker::GetManualLinks() const
    -> std::optional<const std::vector<ManualLink>*> {
  if (!linking_data_.has_value()) {
    return std::nullopt;
  }

  return &linking_data_->manual_links;
}

///
auto Linker::GetCanConnectToPinReason(ne::PinId pin_id) const
    -> std::pair<bool, std::string> {
  if (!linking_data_.has_value()) {
    return {true, {}};
  }

  const auto& diagram = parent_diagram_->GetDiagram();
  const auto& repinning_data = linking_data_->repinning_data;
  const auto is_repinning = repinning_data.has_value();

  if (const auto pin_link = core::Diagram::FindPinLink(diagram, pin_id)) {
    if (const auto pin_of_repinning_link =
            is_repinning &&
            ((*pin_link)->id == repinning_data->link_to_repin)) {
      return {true, {}};
    }

    return {false, "Pin Is Taken"};
  }

  const auto& pin_node = core::Diagram::FindPinNode(diagram, pin_id);
  const auto& dragged_from_pin_data = linking_data_->dragged_from_pin_data;
  const auto source_node = is_repinning ? repinning_data->fixed_pin_data.node_id
                                        : dragged_from_pin_data.node_id;

  if (const auto same_node = pin_node.GetId() == source_node) {
    return {false, "Same Node"};
  }

  const auto pin_kind = core::INode::GetPinKind(pin_node, pin_id);
  const auto same_kind = pin_kind == dragged_from_pin_data.kind;

  if (const auto wrong_kind = is_repinning != same_kind) {
    return {false, "Wrong Pin Kind"};
  }

  if (is_repinning) {
    return {true, "Move Link"};
  }

  return {true, "Create Link"};
}

///
void Linker::CreateLink(ne::PinId source_pin, ne::PinId target_pin) {
  const auto source_pin_kind = GetCurrentLinkSourcePin().kind;

  if (source_pin_kind == ne::PinKind::Input) {
    parent_diagram_->CreateLink(target_pin, source_pin);
  } else {
    parent_diagram_->CreateLink(source_pin, target_pin);
  }
}

///
void Linker::CreateOrRepinCurrentLink(ne::PinId target_pin) {
  Expects(linking_data_.has_value());

  if (const auto is_repinning_link =
          linking_data_->repinning_data.has_value()) {
    parent_diagram_->MoveLink(linking_data_->dragged_from_pin_data.id,
                              target_pin);
  } else {
    CreateLink(GetCurrentLinkSourcePin().id, target_pin);
  }

  linking_data_.reset();
}

///
auto Linker::CreateManualLink(ne::PinId source_pin_id,
                              const PosVariant& target_pos,
                              const ImColor& color) const {
  auto link = ManualLink{.color = color, .thickness = 4};

  const auto& source_pin = GetCurrentLinkSourcePin();
  const auto& source_node =
      Diagram::FindNode(*parent_diagram_, source_pin.node_id);
  const auto source_pos = PosVariant{source_node.GetPinTipPos(source_pin_id)};

  if (source_pin.kind == ne::PinKind::Input) {
    link.start_pos = target_pos;
    link.end_pos = source_pos;
  } else {
    link.start_pos = source_pos;
    link.end_pos = target_pos;
  }

  return link;
}

///
auto Linker::GetRepinningLinkColor() const {
  Expects(linking_data_.has_value());

  if (const auto not_hovering_over_pin =
          !linking_data_->hovering_data.has_value()) {
    return ImColor{1.F, 1.F, 1.F};
  }

  if (const auto can_link_to_hovered_pin = GetCanCreateLinkReason().first) {
    return ImColor{1.F / 2, 1.F, 1.F / 2};
  }

  return ImColor{1.F, 1.F / 2, 1.F / 2};
}

///
void Linker::UpdateManualLinks() {
  Expects(linking_data_.has_value());

  auto& manual_links = linking_data_->manual_links;
  manual_links.clear();

  if (!linking_data_->creating_data.has_value()) {
    if (const auto& repinning_data = linking_data_->repinning_data) {
      const auto repinning_link =
          CreateManualLink(repinning_data->fixed_pin_data.id, MousePos{},
                           GetRepinningLinkColor());

      manual_links.emplace_back(repinning_link);
    }

    return;
  }

  const auto& creating_data = linking_data_->creating_data;
  const auto& new_node_pos = creating_data->new_node_pos;
  const auto creating_link_color = ImColor{1.F, 1.F, 1.F};
  const auto creating_link = CreateManualLink(
      GetCurrentLinkSourcePin().id, new_node_pos, creating_link_color);

  manual_links.emplace_back(creating_link);

  if (!creating_data->create_for_all_pins) {
    return;
  }

  for (const auto empty_source_pin : creating_data->source_node_empty_pins) {
    const auto empty_source_link =
        CreateManualLink(empty_source_pin, new_node_pos, creating_link_color);

    manual_links.emplace_back(empty_source_link);
  }
}
}  // namespace esc::coreui