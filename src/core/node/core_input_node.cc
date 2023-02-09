#include "core_input_node.h"

InputNode::InputNode(esc::IdGenerator& id_generator)
    : Node{id_generator.GetNext<ne::NodeId>(),
           {},
           {{id_generator.GetNext<ne::PinId>(), "", PinType::Flow,
             PinKind::Output, this, true}},
           "Input",
           {255, 0, 0}} {}