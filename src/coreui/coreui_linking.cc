#include "coreui_linking.h"

#include <iostream>

#include "core_i_node.h"
#include "core_link.h"
#include "cpp_assert.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::coreui {
///
Linking::Linking(Callbacks callbacks) : callbacks_{std::move(callbacks)} {}

///
auto Linking::GetRepinningLinkColor() const {
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
auto Linking::GetCurrentLinkSourcePin() const {
  Expects(linking_data_.has_value());

  if (linking_data_->repinning_data.has_value()) {
    return std::pair{linking_data_->repinning_data->fixed_pin,
                     linking_data_->repinning_data->fixed_pin_kind};
  }

  return std::pair{linking_data_->dragged_from_pin,
                   linking_data_->dragged_from_pin_kind};
}

///
auto Linking::CreateManualLinkTo(const PosVariant& target_pos,
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
void Linking::SetPins(const std::optional<ne::PinId>& dragged_from_pin,
                      const std::optional<ne::PinId>& hovering_over_pin) {
  linking_data_.reset();

  if (!dragged_from_pin.has_value()) {
    return;
  }

  const auto& dragged_from_node = callbacks_.find_pin_node(*dragged_from_pin);

  linking_data_ = LinkingData{.dragged_from_pin = *dragged_from_pin,
                              .dragged_from_node = dragged_from_node.GetId(),
                              .dragged_from_pin_kind = core::INode::GetPinKind(
                                  dragged_from_node, *dragged_from_pin)};

  const auto link_to_repin = callbacks_.find_pin_link(*dragged_from_pin);

  if (link_to_repin.has_value()) {
    const auto fixed_pin =
        core::Link::GetOtherPin(**link_to_repin, *dragged_from_pin);

    linking_data_->repinning_data = RepinningData{
        .link_to_repin = (*link_to_repin)->id,
        .fixed_pin = fixed_pin,
        .fixed_pin_kind = core::Link::GetPinKind(**link_to_repin, fixed_pin),
        .fixed_pin_node = callbacks_.find_pin_node(fixed_pin).GetId()};

    linking_data_->manual_link =
        CreateManualLinkTo(MousePos{}, GetRepinningLinkColor());
  }

  if (!hovering_over_pin.has_value()) {
    return;
  }

  const auto& hovering_over_pin_node =
      callbacks_.find_pin_node(*hovering_over_pin);

  linking_data_->hovering_data = HoveringData{
      .hovering_over_pin = *hovering_over_pin,
      .hovering_over_pin_kind =
          core::INode::GetPinKind(hovering_over_pin_node, *hovering_over_pin),
      .reason = GetCanConnectToPinReason(*hovering_over_pin).second};
}

///
auto Linking::CanConnectToPin(ne::PinId pin_id) const -> bool {
  return GetCanConnectToPinReason(pin_id).first;
}

///
auto Linking::CanConnectToNode(const core::INode& node) const -> bool {
  const auto& [source_pin, source_kind] = GetCurrentLinkSourcePin();

  if (source_kind == ne::PinKind::Input) {
    return !node.GetOutputPinIds().empty();
  }

  return node.GetInputPinId().has_value();
}

///
auto Linking::GetCanCreateLinkReason() const -> std::pair<bool, std::string> {
  if (!linking_data_.has_value() || !linking_data_->hovering_data.has_value()) {
    return {true, {}};
  }

  return GetCanConnectToPinReason(
      linking_data_->hovering_data->hovering_over_pin);
}

///
auto Linking::IsRepiningLink(ne::LinkId link_id) const -> bool {
  if (!linking_data_.has_value() ||
      !linking_data_->repinning_data.has_value()) {
    return false;
  }

  return linking_data_->repinning_data->link_to_repin == link_id;
}

///
void Linking::AcceptNewLink() {
  Expects(linking_data_.has_value());
  Expects(linking_data_->hovering_data.has_value());

  if (const auto is_repinning_link =
          linking_data_->repinning_data.has_value()) {
    callbacks_.delete_link(linking_data_->repinning_data->link_to_repin);
  }

  const auto [source_pin, source_kind] = GetCurrentLinkSourcePin();
  const auto target_pin = linking_data_->hovering_data->hovering_over_pin;

  if (source_kind == ne::PinKind::Input) {
    callbacks_.create_link(target_pin, source_pin);
  } else {
    callbacks_.create_link(source_pin, target_pin);
  }

  linking_data_.reset();
}

///
void Linking::StartCreatingNode() {
  Expects(linking_data_.has_value());
  linking_data_->creating_node = true;
  linking_data_->manual_link =
      CreateManualLinkTo(NewNodePos{}, {1.F, 1.F, 1.F});
}

///
auto Linking::IsCreatingNode() const -> bool {
  return linking_data_.has_value() && linking_data_->creating_node;
}

///
void Linking::AcceptNewNode(core::INode& node) {
  const auto [source_pin, source_kind] = GetCurrentLinkSourcePin();
  auto target_pin = ne::PinId{};

  if (source_kind == ne::PinKind::Input) {
    const auto& output_pins = node.GetOutputPinIds();
    Expects(!output_pins.empty());
    target_pin = output_pins[0];
  } else {
    const auto& input_pin = node.GetInputPinId();
    Expects(input_pin.has_value());
    target_pin = *input_pin;
  }

  if (source_kind == ne::PinKind::Input) {
    callbacks_.create_link(target_pin, source_pin);
  } else {
    callbacks_.create_link(source_pin, target_pin);
  }

  linking_data_.reset();
}

///
void Linking::DiscardNewNode() { linking_data_.reset(); }

///
auto Linking::GetManualLink() const -> std::optional<const ManualLink*> {
  if (!linking_data_.has_value() || !linking_data_->manual_link.has_value()) {
    return std::nullopt;
  }

  return &*linking_data_->manual_link;
}

///
auto Linking::GetCanConnectToPinReason(ne::PinId pin_id) const
    -> std::pair<bool, std::string> {
  if (!linking_data_.has_value()) {
    return {true, {}};
  }

  const auto is_repinning = linking_data_->repinning_data.has_value();

  if (const auto pin_link = callbacks_.find_pin_link(pin_id)) {
    if (const auto pin_of_link_being_repinned =
            is_repinning &&
            ((*pin_link)->id == linking_data_->repinning_data->link_to_repin)) {
      return {true, {}};
    }

    return {false, "Pin Is Taken"};
  }

  const auto& pin_node = callbacks_.find_pin_node(pin_id);

  if (const auto same_node =
          pin_node.GetId() == linking_data_->dragged_from_node) {
    return {false, "Same Node"};
  }

  if (is_repinning) {
    if (const auto same_node =
            pin_node.GetId() == linking_data_->repinning_data->fixed_pin_node) {
      return {false, "Same Node"};
    }
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
}  // namespace esc::coreui