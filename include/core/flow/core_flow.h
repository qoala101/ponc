#ifndef VH_CORE_FLOW_H_
#define VH_CORE_FLOW_H_

#include <imgui_node_editor.h>

#include <cstdint>
#include <map>
#include <optional>

namespace esc::core {
// ---
struct Flow {
  // ---
  std::optional<std::pair<uintptr_t, float>> input_pin_flow{};
  // ---
  std::map<uintptr_t, float> output_pin_flows{};

 private:
  // ---
  friend auto operator+(Flow &left, const Flow &right) -> Flow &;
  // ---
  friend auto operator+(Flow &left, float right) -> Flow &;
  // ---
  friend void operator+=(Flow &left, const Flow &right);
  // ---
  friend void operator+=(Flow &left, float right);
};
}  // namespace esc::core

#endif  // VH_CORE_FLOW_H_
