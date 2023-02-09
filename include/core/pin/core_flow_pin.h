#ifndef VH_CORE_FLOW_PIN_H_
#define VH_CORE_FLOW_PIN_H_

#include "core_pin.h"

class FlowPin : public Pin {
 public:
  explicit FlowPin(ne::PinId id, PinKind kind, bool editable);

  auto GetType [[nodiscard]] () const -> PinType override;
};

#endif  // VH_CORE_FLOW_PIN_H_
