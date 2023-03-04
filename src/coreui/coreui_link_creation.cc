#include "coreui_link_creation.h"

#include "core_diagram.h"
#include "core_i_node.h"
#include "cpp_assert.h"
#include "imgui_node_editor.h"

namespace esc::coreui {
///
LinkCreation::LinkCreation(Callbacks callbacks)
    : callbacks_{std::move(callbacks)} {}

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
    const auto fixed_pin = (dragged_from_pin == (*link_to_repin)->start_pin_id)
                               ? (*link_to_repin)->end_pin_id
                               : (*link_to_repin)->start_pin_id;
    creating_data_->repinning_data = RepinningData{
        .link_to_repin = (*link_to_repin)->id,
        .fixed_pin = fixed_pin,
        .fixed_pin_node = callbacks_.find_pin_node(fixed_pin).GetId()};
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
auto LinkCreation::IsCreatingLink() const -> bool {
  return creating_data_.has_value();
}

///
auto LinkCreation::CanConnectToPin(ne::PinId pin_id) const -> bool {
  return GetCanConnectToPinReason(pin_id).first;
}

///
auto LinkCreation::IsHoveringOverPin() const -> bool {
  Expects(creating_data_.has_value());
  return creating_data_->hovering_data.has_value();
}

///
auto LinkCreation::CanCreateLink() const -> bool {
  return GetCanCreateLinkReason().first;
}

///
auto LinkCreation::GetCanCreateLinkReason() const
    -> std::pair<bool, std::string> {
  Expects(creating_data_.has_value());
  Expects(creating_data_->hovering_data.has_value());
  return GetCanConnectToPinReason(
      creating_data_->hovering_data->hovering_over_pin);
}

///
auto LinkCreation::IsRepinningLink() const -> bool {
  Expects(creating_data_.has_value());
  return creating_data_->repinning_data.has_value();
}

///
auto LinkCreation::IsLinkBeingRepinned(ne::LinkId link_id) const -> bool {
  Expects(creating_data_.has_value());
  Expects(creating_data_->repinning_data.has_value());
  return creating_data_->repinning_data->link_to_repin == link_id;
}

///
auto LinkCreation::GetCurrentLinkSourcePin() const
    -> std::pair<ne::PinId, ne::PinKind> {
  Expects(creating_data_.has_value());

  if (creating_data_->repinning_data.has_value()) {
    return {creating_data_->repinning_data->fixed_pin,
            creating_data_->repinning_data->fixed_pin_kind};
  }

  return {creating_data_->dragged_from_pin,
          creating_data_->dragged_from_pin_kind};
}

///
void LinkCreation::AcceptCurrentLink() const {
  Expects(creating_data_.has_value());
  Expects(creating_data_->hovering_data.has_value());

  auto opposite_to_hovering_pin = ne::PinId{};

  if (creating_data_->repinning_data.has_value()) {
    opposite_to_hovering_pin = creating_data_->repinning_data->fixed_pin;

    callbacks_.delete_link(creating_data_->repinning_data->link_to_repin);
  } else {
    opposite_to_hovering_pin = creating_data_->dragged_from_pin;
  }

  if (creating_data_->hovering_data->hovering_over_pin_kind ==
      ne::PinKind::Input) {
    callbacks_.create_link(opposite_to_hovering_pin,
                           creating_data_->hovering_data->hovering_over_pin);
    return;
  }

  callbacks_.create_link(creating_data_->hovering_data->hovering_over_pin,
                         opposite_to_hovering_pin);
}

///
auto LinkCreation::GetCanConnectToPinReason(ne::PinId pin_id) const
    -> std::pair<bool, std::string> {
  Expects(creating_data_.has_value());

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