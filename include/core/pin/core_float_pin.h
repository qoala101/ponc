#ifndef VH_CORE_FLOAT_PIN_H_
#define VH_CORE_FLOAT_PIN_H_

#include "core_pin.h"

class FloatPin : public IPin {
 public:
  explicit FloatPin(ne::PinId id, std::string name, PinKind kind,
                    bool editable);

  auto GetType  () const -> PinType override;

  auto GetValue  () -> float &;
  void SetValue(float value);

 private:
  float value_{};
};

#endif  // VH_CORE_FLOAT_PIN_H_
