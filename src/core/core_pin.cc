#include "core_pin.h"

Pin::Pin(ne::PinId id, std::string name, PinKind kind, bool editable)
    : id_{id},
      ui_data_{.Name = std::move(name), .editable = editable, .Kind = kind} {}

auto Pin::GetId() const -> ne::PinId { return id_; }

void Pin::SetId(ne::PinId id) { id_ = id; }