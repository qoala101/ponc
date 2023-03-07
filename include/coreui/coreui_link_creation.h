#ifndef VH_COREUI_LINK_CREATION_H_
#define VH_COREUI_LINK_CREATION_H_

#include <imgui.h>
#include <imgui_node_editor.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "core_i_family.h"
#include "core_i_node.h"
#include "core_link.h"
#include "coreui_family.h"
#include "cpp_callbacks.h"

namespace esc::coreui {
///
struct MousePos {};

///
using PosVariant = std::variant<MousePos, ne::PinId, ImVec2>;

///
struct HandmadeLink {
  ///
  PosVariant start_pos{};
  ///
  PosVariant end_pos{};
  ///
  ImColor color{};
  ///
  float thickness{};
};

///
class LinkCreation {
 public:
  ///
  struct Callbacks {
    ///
    cpp::Query<const core::INode&, ne::PinId> find_pin_node{};
    ///
    cpp::Query<std::optional<const core::Link*>, ne::PinId> find_pin_link{};
    ///
    cpp::Action<void(std::unique_ptr<core::INode>)> emplace_node{};
    ///
    cpp::Action<void(ne::PinId, ne::PinId)> create_link{};
    ///
    cpp::Action<void(ne::LinkId)> delete_link{};
  };

  ///
  explicit LinkCreation(Callbacks callbacks);

  ///
  void SetPins(const std::optional<ne::PinId>& dragged_from_pin,
               const std::optional<ne::PinId>& hovering_over_pin = {});
  ///
  auto IsCreatingLink() const -> bool;
  ///
  auto CanConnectToPin(ne::PinId pin_id) const -> bool;
  ///
  auto GetCanCreateLinkReason() const -> std::pair<bool, std::string>;
  ///
  auto IsRepinningLink() const -> bool;
  ///
  auto IsLinkBeingRepinned(ne::LinkId link_id) const -> bool;
  ///
  auto GetHandmadeLink() const -> std::optional<HandmadeLink>;
  ///
  void AcceptNewLink();
  ///
  void SetNodeBeingCreatedAt(const ImVec2& pos);
  ///
  auto IsFamilyValidForNewNode(const core::IFamily& family) const -> bool;
  ///
  void AcceptNewNode(std::unique_ptr<core::INode> node);
  ///
  void DiscardNewNode();

 private:
  ///
  struct HoveringData {
    ///
    ne::PinId hovering_over_pin{};
    ///
    ne::PinKind hovering_over_pin_kind{};
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
    ne::PinKind fixed_pin_kind{};
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
    std::optional<ImVec2> new_node_pos{};
    ///
    std::optional<HandmadeLink> handmade_link{};
    ///
    std::optional<HoveringData> hovering_data{};
    ///
    std::optional<RepinningData> repinning_data{};
  };

  ///
  auto GetCurrentLinkSourcePin() const;
  ///
  auto GetHandmadeLinkColor() const;
  ///
  auto CreateHandmadeLinkTo(const PosVariant& target_pos) const;
  ///
  auto GetCanConnectToPinReason(ne::PinId pin_id) const
      -> std::pair<bool, std::string>;

  ///
  Callbacks callbacks_{};
  ///
  std::optional<CreatingData> creating_data_{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_LINK_CREATION_H_
