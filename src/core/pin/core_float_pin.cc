#include "core_float_pin.h"

#include "esc_enums.h"

FloatPin::FloatPin(ne::PinId id, std::string name, PinKind kind, bool editable)
    : IPin{id, std::move(name), kind, editable} {}

auto FloatPin::GetType() const -> PinType { return PinType::Float; }

auto FloatPin::GetValue() -> float& { return value_; }

void FloatPin::SetValue(float value) { value_ = value; }