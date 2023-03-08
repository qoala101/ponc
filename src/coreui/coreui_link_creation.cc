#include "coreui_link_creation.h"

#include <iostream>

#include "core_i_node.h"
#include "core_link.h"
#include "cpp_assert.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::coreui {
///
LinkCreation::LinkCreation(Callbacks callbacks)
    : callbacks_{std::move(callbacks)} {}

///
auto LinkCreation::GetRepinningLinkColor() const {
  Expects(creating_data_.has_value());

  if (const auto not_hovering_over_pin =
          !creating_data_->hovering_data.has_value()) {
    return ImColor{1.F, 1.F, 1.F};
  }

  if (const auto can_create_link_to_hovered_pin =
          GetCanCreateLinkReason().first) {
    return ImColor{1.F / 2, 1.F, 1.F / 2};
  }

  return ImColor{1.F, 1.F / 2, 1.F / 2};
}

///
auto LinkCreation::GetCurrentLinkSourcePin() const {
  Expects(creating_data_.has_value());

  if (creating_data_->repinning_data.has_value()) {
    return std::pair{creating_data_->repinning_data->fixed_pin,
                     creating_data_->repinning_data->fixed_pin_kind};
  }

  return std::pair{creating_data_->dragged_from_pin,
                   creating_data_->dragged_from_pin_kind};
}

///
auto LinkCreation::CreateManualLinkTo(const PosVariant& target_pos,
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
void LinkCreation::SetPins(const std::optional<ne::PinId>& dragged_from_pin,
                           const std::optional<ne::PinId>& hovering_over_pin) {
  creating_data_.reset();

  if (!dragged_from_pin.has_value()) {
    return;
  }

  const auto& dragged_from_node = callbacks_.find_pin_node(*dragged_from_pin);

  creating_data_ =
      CreatingData{.dragged_from_pin = *dragged_from_pin,
                   .dragged_from_node = dragged_from_node.GetId(),
                   .dragged_from_pin_kind = core::INode::GetPinKind(
                       dragged_from_node, *dragged_from_pin)};

  const auto link_to_repin = callbacks_.find_pin_link(*dragged_from_pin);

  if (link_to_repin.has_value()) {
    const auto fixed_pin =
        core::Link::GetOtherPin(**link_to_repin, *dragged_from_pin);

    creating_data_->repinning_data = RepinningData{
        .link_to_repin = (*link_to_repin)->id,
        .fixed_pin = fixed_pin,
        .fixed_pin_kind = core::Link::GetPinKind(**link_to_repin, fixed_pin),
        .fixed_pin_node = callbacks_.find_pin_node(fixed_pin).GetId()};

    creating_data_->manual_link =
        CreateManualLinkTo(MousePos{}, GetRepinningLinkColor());
  }

  if (!hovering_over_pin.has_value()) {
    return;
  }

  const auto& hovering_over_pin_node =
      callbacks_.find_pin_node(*hovering_over_pin);

  creating_data_->hovering_data = HoveringData{
      .hovering_over_pin = *hovering_over_pin,
      .hovering_over_pin_kind =
          core::INode::GetPinKind(hovering_over_pin_node, *hovering_over_pin),
      .reason = GetCanConnectToPinReason(*hovering_over_pin).second};
}

///
auto LinkCreation::CanConnectToPin(ne::PinId pin_id) const -> bool {
  return GetCanConnectToPinReason(pin_id).first;
}

///
auto LinkCreation::CanConnectToNode(const core::INode& node) const -> bool {
  const auto& [source_pin, source_kind] = GetCurrentLinkSourcePin();

  if (source_kind == ne::PinKind::Input) {
    return !node.GetOutputPinIds().empty();
  }

  return node.GetInputPinId().has_value();
}

///
auto LinkCreation::GetCanCreateLinkReason() const
    -> std::pair<bool, std::string> {
  if (!creating_data_.has_value() ||
      !creating_data_->hovering_data.has_value()) {
    return {true, {}};
  }

  return GetCanConnectToPinReason(
      creating_data_->hovering_data->hovering_over_pin);
}

///
auto LinkCreation::IsRepiningLink(ne::LinkId link_id) const -> bool {
  if (!creating_data_.has_value() ||
      !creating_data_->repinning_data.has_value()) {
    return false;
  }

  return creating_data_->repinning_data->link_to_repin == link_id;
}

///
void LinkCreation::AcceptNewLink() {
  Expects(creating_data_.has_value());
  Expects(creating_data_->hovering_data.has_value());

  if (const auto is_repinning_link =
          creating_data_->repinning_data.has_value()) {
    callbacks_.delete_link(creating_data_->repinning_data->link_to_repin);
  }

  const auto [source_pin, source_kind] = GetCurrentLinkSourcePin();
  const auto target_pin = creating_data_->hovering_data->hovering_over_pin;

  if (source_kind == ne::PinKind::Input) {
    callbacks_.create_link(target_pin, source_pin);
  } else {
    callbacks_.create_link(source_pin, target_pin);
  }

  creating_data_.reset();
}

///
void LinkCreation::StartCreatingNode() {
  Expects(creating_data_.has_value());
  creating_data_->creating_node = true;
  creating_data_->manual_link =
      CreateManualLinkTo(NewNodePos{}, {1.F, 1.F, 1.F});
}

///
auto LinkCreation::IsCreatingNode() const -> bool {
  return creating_data_.has_value() && creating_data_->creating_node;
}

///
void LinkCreation::AcceptNewNode(core::INode& node) {
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

  creating_data_.reset();
}

///
void LinkCreation::DiscardNewNode() { creating_data_.reset(); }

///
auto LinkCreation::GetManualLink() const -> std::optional<const ManualLink*> {
  if (!creating_data_.has_value() || !creating_data_->manual_link.has_value()) {
    return std::nullopt;
  }

  return &*creating_data_->manual_link;
}

///
auto LinkCreation::GetCanConnectToPinReason(ne::PinId pin_id) const
    -> std::pair<bool, std::string> {
  if (!creating_data_.has_value()) {
    return {true, {}};
  }

  const auto is_repinning = creating_data_->repinning_data.has_value();

  if (const auto pin_link = callbacks_.find_pin_link(pin_id)) {
    if (const auto pin_of_link_being_repinned =
            is_repinning && ((*pin_link)->id ==
                             creating_data_->repinning_data->link_to_repin)) {
      return {true, {}};
    }

    return {false, "Pin Is Taken"};
  }

  const auto& pin_node = callbacks_.find_pin_node(pin_id);

  if (const auto same_node =
          pin_node.GetId() == creating_data_->dragged_from_node) {
    return {false, "Same Node"};
  }

  if (is_repinning) {
    if (const auto same_node = pin_node.GetId() ==
                               creating_data_->repinning_data->fixed_pin_node) {
      return {false, "Same Node"};
    }
  }

  const auto pin_kind = core::INode::GetPinKind(pin_node, pin_id);
  const auto same_kind = pin_kind == creating_data_->dragged_from_pin_kind;

  if (const auto wrong_kind = is_repinning != same_kind) {
    return {false, "Wrong Pin Kind"};
  }

  if (is_repinning) {
    return {true, "Move Link"};
  }

  return {true, "Create Link"};
}
}  // namespace esc::coreui