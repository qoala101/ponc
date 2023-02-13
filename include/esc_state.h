#ifndef VH_ESC_STATE_H_
#define VH_ESC_STATE_H_

#include <imgui_node_editor.h>

#include <functional>

#include "core_app.h"
#include "core_flow_calculator.h"
#include "core_id_generator.h"

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
  } drawing_{};

 private:
  // ---
  void ExecuteEvents();

  std::vector<std::function<void(State &state)>> events_{};
};
}  // namespace esc

#endif  // VH_ESC_STATE_H_
