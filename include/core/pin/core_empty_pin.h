#ifndef VH_CORE_EMPTY_PIN_H_
#define VH_CORE_EMPTY_PIN_H_

#include "core_pin.h"
#include "esc_enums.h"

class EmptyPin : public Pin {
 public:
  explicit EmptyPin(ne::PinId id, PinKind kind);

  auto GetType  () const -> PinType override;
};

#endif  // VH_CORE_EMPTY_PIN_H_
