#ifndef VH_COREUI_CREATION_H_
#define VH_COREUI_CREATION_H_

#include <imgui_node_editor.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "core_diagram.h"
#include "core_i_node.h"
#include "core_link.h"

namespace esc::coreui {
class LinkCreation {
 public:
  using SignalGetDiagram = std::function<auto()->const core::Diagram&>;
  using SignalCreateLink =
      std::function<void(ne::PinId start_pin_id, ne::PinId end_pin_id)>;
  using SignalDeleteLink = std::function<void(ne::LinkId link_id)>;

  LinkCreation(SignalGetDiagram signal_get_diagram,
               SignalCreateLink signal_create_link,
               SignalDeleteLink signal_delete_link);

  void SetPins(const std::optional<ne::PinId>& dragged_from_pin,
               const std::optional<ne::PinId>& hovering_over_pin = {});

  auto IsCreatingLink() const -> bool;
  auto CanConnectToPin(ne::PinId pin_id) const -> bool;

  auto IsHoveringOverPin() const -> bool;
  auto GetCanCreateLinkReason() const -> std::pair<bool, std::string>;

  auto IsRepinningLink() const -> bool;
  auto IsLinkBeingRepinned(ne::LinkId link_id) const -> bool;
  auto GetFixedPinOfLinkBeingRepinned() const -> ne::PinId;

  //
  void AcceptCurrentLink();
  void AcceptNewNode();

 private:
  auto GetCanConnectToPinReason(ne::PinId pin_id) const
      -> std::pair<bool, std::string>;

  SignalGetDiagram signal_get_diagram_{};
  SignalCreateLink signal_create_link_{};
  SignalDeleteLink signal_delete_link_{};

  struct Hovering {
    ne::PinId hovering_over_pin{};
    bool can_connect_to_hovered_pin{};
    std::string reason{};
  };

  struct Repinning {
    ne::LinkId link_to_repin{};
    ne::PinId fixed_pin{};
    const core::INode* fixed_pin_node{};
  };

  struct Creating {
    ne::PinId dragged_from_pin{};
    const core::INode* dragged_from_node{};
    ne::PinKind dragged_from_pin_kind{};

    std::optional<Hovering> hovering{};
    std::optional<Repinning> repinning{};
  };

  std::optional<Creating> creating_{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_CREATION_H_
