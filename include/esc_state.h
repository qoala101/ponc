#ifndef VH_ESC_STATE_H_
#define VH_ESC_STATE_H_

#include <imgui_node_editor.h>

#include <functional>

#include "core_app.h"
#include "core_flow_calculator.h"
#include "core_id_generator.h"
#include "imgui.h"

namespace esc {
// ---
class State {
 public:
  // ---
  State();

  // ---
  static void OpenDiagramFromFile(State &state, const std::string &file_path);
  // ---
  static void SaveDiagramToFile(const State &state,
                                const std::string &file_path);
  // ---
  static void ResetDiagram(State &state);
  // ---
  static void EraseLink(State &state, ne::LinkId link_id);
  // ---
  static void EraseNodeAndConnectedLinks(State &state, ne::NodeId node_id);

  static void ReplaceWithPlaceholder(State &state, ne::NodeId node_id);
  static void MakeGroupFromSelectedNodes(State &state, std::string group_name);

  // ---
  void OnFrame();
  // ---
  void PostEvent(std::function<void(State &state)> event);

  //  private:
  core::App app_{};
  core::IdGenerator id_generator_{};
  core::FlowCalculator flow_calculator_{};

  struct {
    std::optional<ne::PinId> not_yet_connected_pin_of_new_link_id{};
    std::optional<ne::PinId> connect_new_node_to_existing_pin_id{};

    ImVec2 pinned_pin_pos{};
    std::optional<ne::NodeId> popup_node_{};

    struct {
      bool color_flow{};
      float min{-27};
      float low{-22};
      float high{-18};
      float max{6};
    } link_colors{};
  } drawing_{};

  auto GetExistingLinkFromSamePin() -> const core::Link * {
    if (!drawing_.not_yet_connected_pin_of_new_link_id.has_value()) {
      return nullptr;
    }

    const auto new_link_starts_from_pin =
        *drawing_.not_yet_connected_pin_of_new_link_id;

    return app_.GetDiagram().FindLinkFromPin(new_link_starts_from_pin);
  }

  auto GetDrawLINEFromPin() -> std::optional<ne::PinId> {
    const auto *existing_link_from_same_pin = GetExistingLinkFromSamePin();

    if (existing_link_from_same_pin == nullptr) {
      return std::nullopt;
    }

    const auto new_link_starts_from_pin =
        *drawing_.not_yet_connected_pin_of_new_link_id;

    return (existing_link_from_same_pin->start_pin_id ==
            new_link_starts_from_pin)
               ? existing_link_from_same_pin->end_pin_id
               : existing_link_from_same_pin->start_pin_id;
  }

  auto GetColorForFlowValue(float value) const -> ImColor;

 private:
  // ---
  void ExecuteEvents();

  std::vector<std::function<void(State &state)>> events_{};
};
}  // namespace esc

#endif  // VH_ESC_STATE_H_
