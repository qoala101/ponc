#include "core_pin.h"

IPin::IPin(ne::PinId id, std::string name, PinKind kind, bool editable)
    : id_{id},
      ui_data_{.Name = std::move(name), .editable = editable, .Kind = kind} {}

auto IPin::GetId() const -> ne::PinId { return id_; }

void IPin::SetId(ne::PinId id) { id_ = id; }