#include "core_pin.h"

Pin::Pin(ne::PinId id, std::string name, PinType type, PinKind kind, 
         bool editable)
    : ID{id},
      Type{type},
      Kind{kind},
      ui_data_{.Name = std::move(name), .editable = editable} {}