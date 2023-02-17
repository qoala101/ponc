#ifndef VH_CORE_STATE_H_
#define VH_CORE_STATE_H_

#include "core_diagram.h"
#include "flow_calculator.h"

namespace esc::core {
struct CoreState {
  Diagram diagram_{};
  IdGenerator id_generator_{};
  flow::FlowCalculator flow_calculator_{};
};
}  // namespace esc::core

#endif  // VH_CORE_STATE_H_
