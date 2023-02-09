#include "core_input_node.h"
#include "core_flow_pin.h"

InputNode::InputNode(esc::IdGenerator& id_generator)
    : Node{id_generator.GetNext<ne::NodeId>(),
           {},
           {std::make_shared<FlowPin>(id_generator.GetNext<ne::PinId>(),
                                      PinKind::Output, true)},
           "Input",
           {255, 0, 0}} {}