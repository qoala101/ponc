#include "core_client_node.h"

#include "esc_id_generator.h"

ClientNode::ClientNode(esc::IdGenerator& id_generator)
    : Node{id_generator.GetNext<ne::NodeId>(),
           {{id_generator.GetNext<ne::PinId>(), "", PinType::Flow,
             PinKind::Input, false},
            {id_generator.GetNext<ne::PinId>(), "min", PinType::Float,
             PinKind::Input, true},
            {id_generator.GetNext<ne::PinId>(), "max", PinType::Float,
             PinKind::Input, true}},
           {},
           "Client",
           {0, 255, 0}} {}