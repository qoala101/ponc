#include "core_pin.h"

Pin::Pin(ne::PinId id, std::string name, PinType type, PinKind kind, Node* node)
    : ID{id},
      node{node},
      Name{std::move(name)},
      Type{type},
      Kind{kind} {}