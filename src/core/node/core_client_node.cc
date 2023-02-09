#include "core_client_node.h"

#include <memory>

#include "core_float_pin.h"
#include "core_flow_pin.h"
#include "esc_id_generator.h"

ClientNode::ClientNode(esc::IdGenerator& id_generator)
    : Node{id_generator.GetNext<ne::NodeId>(),
           {std::make_shared<FlowPin>(id_generator.GetNext<ne::PinId>(),
                                      PinKind::Input, false),
            std::make_shared<FloatPin>(id_generator.GetNext<ne::PinId>(), "min",
                                       PinKind::Input, true),
            std::make_shared<FloatPin>(id_generator.GetNext<ne::PinId>(), "max",
                                       PinKind::Input, true)},
           {},
           "Client",
           {0, 255, 0}} {}