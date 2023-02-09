#include "core_flow_pin.h"

#include "esc_enums.h"

FlowPin::FlowPin(ne::PinId id, PinKind kind, bool editable)
    : Pin{id, "", kind, editable} {}

auto FlowPin::GetType() const -> PinType { return PinType::Flow; }