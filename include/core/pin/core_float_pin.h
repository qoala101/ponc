#ifndef VH_CORE_FLOAT_PIN_H_
#define VH_CORE_FLOAT_PIN_H_

#include "core_pin.h"

class FloatPin : public Pin {
 public:
  explicit FloatPin(ne::PinId id, std::string name, PinKind kind,
                    bool editable);

  auto GetType [[nodiscard]] () const -> PinType override;
};

#endif  // VH_CORE_FLOAT_PIN_H_
