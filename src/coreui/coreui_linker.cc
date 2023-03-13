#include "coreui_linker.h"

#include <iostream>

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
auto Linker::GetCurrentLinkSourcePin() const {
  Expects(linking_data_.has_value());

  if (linking_data_->repinning_data.has_value()) {
    return std::pair{linking_data_->repinning_data->fixed_pin,
                     linking_data_->repinning_data->fixed_pin_kind};
  }

  return std::pair{linking_data_->dragged_from_pin,
                   linking_data_->dragged_from_pin_kind};
}

///
auto Linker::CreateManualLinkTo(const PosVariant& target_pos,
                                const ImColor& color) const {
  auto link = ManualLink{.color = color, .thickness = 4};

  const auto [source_pin, source_kind] = GetCurrentLinkSourcePin();
  const auto source_pos = PosVariant{source_pin};

  if (source_kind == ne::PinKind::Input) {
    link.start_pos = target_pos;
    link.end_pos = source_pos;
  } else {
    link.start_pos = source_pos;
    link.end_pos = target_pos;
  }

  return link;
}

///
void Linker::SetPins(const std::optional<ne::PinId>& dragged_from_pin,
                     const std::optional<ne::PinId>& hovering_over_pin) {
  linking_data_.reset();

  if (!dragged_from_pin.has_value()) {
    return;
  }

  const auto& diagram = parent_diagram_->GetDiagram();
  const auto& dragged_from_node =
      core::Diagram::FindPinNode(diagram, *dragged_from_pin);

  linking_data_ = LinkingData{.dragged_from_pin = *dragged_from_pin,
                              .dragged_from_node = dragged_from_node.GetId(),
                              .dragged_from_pin_kind = core::INode::GetPinKind(
                                  dragged_from_node, *dragged_from_pin)};

  const auto link_to_repin =
      core::Diagram::FindPinLink(diagram, *dragged_from_pin);

  if (link_to_repin.has_value()) {
    const auto fixed_pin =
        core::Link::GetOtherPin(**link_to_repin, *dragged_from_pin);

    linking_data_->repinning_data = RepinningData{
        .link_to_repin = (*link_to_repin)->id,
        .fixed_pin = fixed_pin,
        .fixed_pin_kind = core::Link::GetPinKind(**link_to_repin, fixed_pin),
        .fixed_pin_node =
            core::Diagram::FindPinNode(diagram, fixed_pin).GetId()};

    linking_data_->manual_link =
        CreateManualLinkTo(MousePos{}, GetRepinningLinkColor());
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
auto Linker::CanConnectToNode(const core::INode& node) const -> bool {
  if (!linking_data_.has_value()) {
    return true;
  }

  const auto& [source_pin, source_kind] = GetCurrentLinkSourcePin();
  const auto target_kind = core::Pin::GetOppositeKind(source_kind);

  return core::INode::FindFirstPinOfKind(node, target_kind).has_value();
}

///
auto Linker::GetCanCreateLinkReason() const -> std::pair<bool, std::string> {
  if (!linking_data_.has_value() || !linking_data_->hovering_data.has_value()) {
    return {true, {}};
  }

  return GetCanConnectToPinReason(
      linking_data_->hovering_data->hovering_over_pin);
}

///
auto Linker::IsRepiningLink(ne::LinkId link_id) const -> bool {
  if (!linking_data_.has_value() ||
      !linking_data_->repinning_data.has_value()) {
    return false;
  }

  return linking_data_->repinning_data->link_to_repin == link_id;
}

///
void Linker::AcceptNewLink() {
  Expects(linking_data_.has_value());
  Expects(linking_data_->hovering_data.has_value());

  AcceptNewLinkTo(linking_data_->hovering_data->hovering_over_pin);
}

///
void Linker::StartCreatingNode() {
  Expects(linking_data_.has_value());
  linking_data_->creating_node = true;
  linking_data_->manual_link =
      CreateManualLinkTo(NewNodePos{}, {1.F, 1.F, 1.F});
}

///
auto Linker::IsCreatingNode() const -> bool {
  return linking_data_.has_value() && linking_data_->creating_node;
}

///
void Linker::AcceptNewNode(core::INode& node) {
  const auto [source_pin, source_kind] = GetCurrentLinkSourcePin();
  const auto target_kind = core::Pin::GetOppositeKind(source_kind);

  AcceptNewLinkTo(core::INode::GetFirstPinOfKind(node, target_kind));
}

///
void Linker::DiscardNewNode() { linking_data_.reset(); }

///
auto Linker::GetManualLink() const -> std::optional<const ManualLink*> {
  if (!linking_data_.has_value() || !linking_data_->manual_link.has_value()) {
    return std::nullopt;
  }

  return &*linking_data_->manual_link;
}

///
auto Linker::GetCanConnectToPinReason(ne::PinId pin_id) const
    -> std::pair<bool, std::string> {
  if (!linking_data_.has_value()) {
    return {true, {}};
  }

  const auto& diagram = parent_diagram_->GetDiagram();
  const auto is_repinning = linking_data_->repinning_data.has_value();

  if (const auto pin_link = core::Diagram::FindPinLink(diagram, pin_id)) {
    if (const auto pin_of_link_being_repinned =
            is_repinning &&
            ((*pin_link)->id == linking_data_->repinning_data->link_to_repin)) {
      return {true, {}};
    }

    return {false, "Pin Is Taken"};
  }

  const auto& pin_node = core::Diagram::FindPinNode(diagram, pin_id);
  const auto source_node = is_repinning
                               ? linking_data_->repinning_data->fixed_pin_node
                               : linking_data_->dragged_from_node;

  if (const auto same_node = pin_node.GetId() == source_node) {
    return {false, "Same Node"};
  }

  const auto pin_kind = core::INode::GetPinKind(pin_node, pin_id);
  const auto same_kind = pin_kind == linking_data_->dragged_from_pin_kind;

  if (const auto wrong_kind = is_repinning != same_kind) {
    return {false, "Wrong Pin Kind"};
  }

  if (is_repinning) {
    return {true, "Move Link"};
  }

  return {true, "Create Link"};
}

///
void Linker::AcceptNewLinkTo(ne::PinId target_pin) {
  Expects(linking_data_.has_value());

  if (const auto is_repinning_link =
          linking_data_->repinning_data.has_value()) {
    parent_diagram_->DeleteLink(linking_data_->repinning_data->link_to_repin);
  }

  const auto [source_pin, source_kind] = GetCurrentLinkSourcePin();

  if (source_kind == ne::PinKind::Input) {
    parent_diagram_->CreateLink(target_pin, source_pin);
  } else {
    parent_diagram_->CreateLink(source_pin, target_pin);
  }

  linking_data_.reset();
}
}  // namespace esc::coreui