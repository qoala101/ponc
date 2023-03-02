#ifndef VH_COREUI_LINK_CREATION_H_
#define VH_COREUI_LINK_CREATION_H_

#include <imgui_node_editor.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "core_i_node.h"
#include "core_link.h"

namespace esc::coreui {
///
class LinkCreation {
 public:
  ///
  struct Hooks {
    ///
    std::function<auto(ne::PinId pin_id)->const core::INode&> find_pin_node{};
    ///
    std::function<auto(ne::PinId pin_id)->std::optional<const core::Link*>>
        find_pin_link{};
  };

  ///
  explicit LinkCreation(Hooks hooks);

  ///
  void SetPins(const std::optional<ne::PinId>& dragged_from_pin,
               const std::optional<ne::PinId>& hovering_over_pin = {});
  ///
  auto IsCreatingLink() const -> bool;
  ///
  auto CanConnectToPin(ne::PinId pin_id) const -> bool;
  ///
  auto IsHoveringOverPin() const -> bool;
  ///
  auto CanCreateLink() const -> bool;
  ///
  auto GetCanCreateLinkReason() const -> std::pair<bool, std::string>;
  ///
  auto IsRepinningLink() const -> bool;
  ///
  auto IsLinkBeingRepinned(ne::LinkId link_id) const -> bool;
  ///
  auto GetFixedPinOfLinkBeingRepinned() const -> ne::PinId;

 private:
  ///
  struct HoveringData {
    ///
    ne::PinId hovering_over_pin{};
    ///
    bool can_connect_to_hovered_pin{};
    ///
    std::string reason{};
  };

  ///
  struct RepinningData {
    ///
    ne::LinkId link_to_repin{};
    ///
    ne::PinId fixed_pin{};
    ///
    ne::NodeId fixed_pin_node{};
  };

  ///
  struct CreatingData {
    ///
    ne::PinId dragged_from_pin{};
    ///
    ne::NodeId dragged_from_node{};
    ///
    ne::PinKind dragged_from_pin_kind{};
    ///
    std::optional<HoveringData> hovering_data{};
    ///
    std::optional<RepinningData> repinning_data{};
  };

  ///
  auto GetCanConnectToPinReason(ne::PinId pin_id) const
      -> std::pair<bool, std::string>;

  ///
  Hooks hooks_{};
  ///
  std::optional<CreatingData> creating_data_{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_LINK_CREATION_H_
