#include "coreui_link_creation.h"

#include "core_i_node.h"
#include "cpp_assert.h"

namespace esc::coreui {
LinkCreation::LinkCreation(SignalGetDiagram signal_get_diagram,
                           SignalCreateLink signal_create_link,
                           SignalDeleteLink signal_delete_link)
    : signal_get_diagram_{std::move(signal_get_diagram)},
      signal_create_link_{std::move(signal_create_link)},
      signal_delete_link_{std::move(signal_delete_link)} {}

void LinkCreation::SetPins(const std::optional<ne::PinId>& dragged_from_pin,
                           const std::optional<ne::PinId>& hovering_over_pin) {
  creating_.reset();

  if (!dragged_from_pin.has_value()) {
    return;
  }

  const auto& diagram = signal_get_diagram_();
  const auto dragged_from_node = FindPinNode(diagram, *dragged_from_pin);

  creating_ = Creating{.dragged_from_pin = *dragged_from_pin,
                       .dragged_from_node = &*dragged_from_node,
                       .dragged_from_pin_kind = core::GetPinKind(
                           *dragged_from_node, *dragged_from_pin)};

  const auto link_to_repin = FindPinLink(diagram, *dragged_from_pin);

  if (link_to_repin.has_value()) {
    const auto fixed_pin = (dragged_from_pin == (*link_to_repin)->start_pin_id)
                               ? (*link_to_repin)->end_pin_id
                               : (*link_to_repin)->start_pin_id;
    creating_->repinning =
        Repinning{.link_to_repin = *link_to_repin,
                  .fixed_pin = fixed_pin,
                  .fixed_pin_node = &*FindPinNode(diagram, fixed_pin)};
  }

  if (!hovering_over_pin.has_value()) {
    return;
  }

  creating_->hovering =
      Hovering{.hovering_over_pin = *hovering_over_pin,
               .reason = GetCanConnectToPinReason(*hovering_over_pin).second};
}

auto LinkCreation::IsCreatingLink() const -> bool {
  return creating_.has_value();
}

auto LinkCreation::CanConnectToPin(ne::PinId pin_id) const -> bool {
  return GetCanConnectToPinReason(pin_id).first;
}

auto LinkCreation::IsHoveringOverPin() const -> bool {
  Expects(creating_.has_value());
  return creating_->hovering.has_value();
}

auto LinkCreation::GetCanCreateLinkReason() const
    -> std::pair<bool, std::string> {
  Expects(creating_.has_value());
  Expects(creating_->hovering.has_value());
  return GetCanConnectToPinReason(creating_->hovering->hovering_over_pin);
}

auto LinkCreation::IsRepinningLink() const -> bool {
  Expects(creating_.has_value());
  return creating_->repinning.has_value();
}

auto LinkCreation::IsLinkBeingRepinned(ne::LinkId link_id) const -> bool {
  Expects(creating_.has_value());
  Expects(creating_->repinning.has_value());
  return creating_->repinning->link_to_repin->id == link_id;
}

auto LinkCreation::GetFixedPinOfLinkBeingRepinned() const -> ne::PinId {
  Expects(creating_.has_value());
  Expects(creating_->repinning.has_value());
  return creating_->repinning->fixed_pin;
}

void LinkCreation::AcceptCurrentLink() {
  // const auto hovering_over_node = FindPinNode(diagram,
  // creating_->hovering->hovering_over_pin); const auto hovering_over_pin_kind
  // =
  //     core::GetPinKind(*hovering_over_node,
  //     creating_->hovering->hovering_over_pin);

  // if (creating_->repinning.has_value()) {
  //   if (hovering_over_pin_kind == ne::PinKind::Input) {
  //     start_pin_id = *fixed_pin;
  //     end_pin_id = *hovering_over_pin_;
  //   } else {
  //     start_pin_id = *hovering_over_pin_;
  //     end_pin_id = *fixed_pin;
  //   }
  // } else {
  //   if (hovering_over_pin_kind == ne::PinKind::Input) {
  //     start_pin_id = *dragged_from_pin_;
  //     end_pin_id = *hovering_over_pin_;
  //   } else {
  //     start_pin_id = *hovering_over_pin_;
  //     end_pin_id = *dragged_from_pin_;
  //   }
  // }

  // if (delete_link.has_value()) {
  //   signal_delete_link_(*delete_link);
  // }

  // signal_create_link_(start_pin_id, end_pin_id);
}

void LinkCreation::AcceptNewNode() {}

auto LinkCreation::GetCanConnectToPinReason(ne::PinId pin_id) const
    -> std::pair<bool, std::string> {
  Expects(creating_.has_value());

  const auto& diagram = signal_get_diagram_();
  const auto is_repinning = creating_->repinning.has_value();

  if (const auto pin_link = core::FindPinLink(diagram, pin_id)) {
    if (const auto pin_of_link_being_repinned =
            is_repinning &&
            ((*pin_link)->id == creating_->repinning->link_to_repin->id)) {
      return {true, {}};
    }

    return {false, "Pin Is Taken"};
  }

  const auto& pin_node = FindPinNode(diagram, pin_id);

  if (const auto same_node =
          pin_node->GetId() == creating_->dragged_from_node->GetId()) {
    return {false, "Same Node"};
  }

  if (is_repinning) {
    if (const auto same_node = pin_node->GetId() ==
                               creating_->repinning->fixed_pin_node->GetId()) {
      return {false, "Same Node"};
    }
  }

  const auto pin_kind = core::GetPinKind(*pin_node, pin_id);
  const auto same_kind = pin_kind == creating_->dragged_from_pin_kind;

  if (const auto wrong_kind = is_repinning != same_kind) {
    return {false, "Wrong Pin Kind"};
  }

  if (is_repinning) {
    return {true, "Move Link"};
  }

  return {true, "Create Link"};
}
}  // namespace esc::coreui