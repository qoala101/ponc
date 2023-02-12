#ifndef VH_ESC_STATE_H_
#define VH_ESC_STATE_H_

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
  void OnFrame();
  // ---
  void PostEvent(std::function<void(State &state)> event);

  //  private:
  core::App app_{};
  core::IdGenerator id_generator_{};
  core::FlowCalculator flow_calculator_{};

 private:
  void ExecuteEvents();

  std::vector<std::function<void(State &state)>> events_{};
};
}  // namespace esc

#endif  // VH_ESC_STATE_H_
