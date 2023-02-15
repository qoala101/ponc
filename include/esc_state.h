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

  struct Rebind {
    ne::PinId fixed_pin{};
    std::optional<ImVec2> fixed_pin_pos{};
  };

  struct NewLink {
    ne::PinId pin_dragged_from{};
    std::optional<ne::PinId> pin_hovered_over{};
    std::optional<Rebind> rebind{};
  };

  struct {
    std::optional<ne::NodeId> popup_node_{};

    struct {
      bool color_flow{};
      float min{-27};
      float low{-22};
      float high{-18};
      float max{6};
    } link_colors{};

    std::optional<NewLink> new_link{};
  } drawing_{};

  auto GetColorForFlowValue(float value) const -> ImColor;

 private:
  // ---
  void ExecuteEvents();

  std::vector<std::function<void(State &state)>> events_{};
};
}  // namespace esc

#endif  // VH_ESC_STATE_H_
