#include "core_empty_pin.h"

#include "esc_enums.h"

EmptyPin::EmptyPin(ne::PinId id, PinKind kind) : Pin{id, "", kind, false} {}

auto EmptyPin::GetType() const -> PinType { return PinType::Empty; }