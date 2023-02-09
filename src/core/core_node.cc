#include "core_node.h"

Node::Node(ne::NodeId id, std::string name, ImColor color)
    : ID{id}, Name{std::move(name)}, Color{color}, Size{0, 0} {}