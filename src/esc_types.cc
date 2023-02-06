#include "esc_types.h"

#include <cstdint>

namespace ed = ax::NodeEditor;

Pin::Pin(int id, const char* name, PinType type)
    : ID{static_cast<uint64_t>(id)},
      Node{nullptr},
      Name{name},
      Type{type},
      Kind{PinKind::Input} {}

Node::Node(int id, const char* name, ImColor color)
    : ID{static_cast<uint64_t>(id)},
      Name{name},
      Color{color},
      Type{NodeType::Blueprint},
      Size{0, 0} {}

Link::Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId)
    : ID{id},
      StartPinID{startPinId},
      EndPinID{endPinId},
      Color{255, 255, 255} {}

auto NodeIdLess::operator()(const ed::NodeId& lhs, const ed::NodeId& rhs) const
    -> bool {
  return lhs.AsPointer() < rhs.AsPointer();
}