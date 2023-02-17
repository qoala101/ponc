#ifndef VH_CORE_STATE_H_
#define VH_CORE_STATE_H_

#include "core_diagram.h"
#include "core_flow_calculator.h"

namespace esc::core {
struct CoreState {
  Diagram diagram_{};
  IdGenerator id_generator_{};
  FlowCalculator flow_calculator_{};
};
}  // namespace esc::core

#endif  // VH_CORE_STATE_H_
