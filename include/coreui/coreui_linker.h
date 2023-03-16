#ifndef VH_COREUI_LINKER_H_
#define VH_COREUI_LINKER_H_

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
#include "coreui_event.h"
#include "coreui_family.h"
#include "cpp_safe_ptr.h"

namespace esc::coreui {
///
class Diagram;

///
struct MousePos {};

///
using PosVariant = std::variant<MousePos, ImVec2>;

///
struct ManualLink {
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
class Linker {
 public:
  ///
  explicit Linker(cpp::SafePtr<Diagram> parent_diagram);

  ///
  void SetPins(const std::optional<ne::PinId>& dragged_from_pin,
               const std::optional<ne::PinId>& hovering_over_pin = {});
  ///
  auto CanConnectToPin(ne::PinId pin_id) const -> bool;
  ///
  auto CanConnectToFamily(const core::IFamily& family) const -> bool;
  ///
  auto GetCanCreateLinkReason() const -> std::pair<bool, std::string>;
  ///
  auto IsRepiningLink(ne::LinkId link_id) const -> bool;
  ///
  auto GetManualLinks() const -> std::vector<ManualLink>;
  ///
  void AcceptCreateLink();
  ///
  void StartCreatingNodeAt(const ImVec2& new_node_pos);
  ///
  auto CanCreateNodeForAllPins() const -> bool;
  ///
  void SetCreateNodeForAllPins(bool create_for_all_pins);
  ///
  void AcceptCreateNode(const Family& family);
  ///
  void DiscardCreateNode();

 private:
  ///
  struct PinData {
    ///
    ne::PinId id{};
    ///
    ne::PinKind kind{};
    ///
    ne::NodeId node_id{};
  };

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
    PinData fixed_pin_data{};
  };

  ///
  struct CreatingData {
    ///
    ImVec2 new_node_pos{};
    ///
    bool create_for_all_pins{};
    ///
    std::vector<ne::PinId> source_node_empty_pins{};
  };

  ///
  struct LinkingData {
    ///
    PinData dragged_from_pin_data{};
    ///
    std::vector<ManualLink> manual_links{};
    ///
    std::optional<HoveringData> hovering_data{};
    ///
    std::optional<RepinningData> repinning_data{};
    ///
    std::optional<CreatingData> creating_data{};
  };

  ///
  auto GetSourcePinData() const -> auto&;
  ///
  auto GetCanConnectToPinReason(ne::PinId pin_id) const
      -> std::pair<bool, std::string>;
  ///
  auto CreateManualLink(ne::PinId source_pin, const PosVariant& target_pos,
                        const ImColor& color) const;
  ///
  auto GetRepinningLinkColor() const;
  ///
  void UpdateManualLinks();
  ///
  auto CreateLink(ne::PinId source_pin, ne::PinId target_pin) const -> Event&;

  ///
  cpp::SafePtr<Diagram> parent_diagram_;
  ///
  std::optional<LinkingData> linking_data_{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_LINKER_H_
