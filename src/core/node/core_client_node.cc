#include "core_client_node.h"

#include "esc_id_generator.h"

ClientNode::ClientNode(esc::IdGenerator& id_generator)
    : Node{id_generator.GetNext<ne::NodeId>(),
           {{id_generator.GetNext<ne::PinId>(), "In", PinType::Flow,
             PinKind::Input, this, false},
            {id_generator.GetNext<ne::PinId>(), "min", PinType::Float,
             PinKind::Input, this, true},
            {id_generator.GetNext<ne::PinId>(), "max", PinType::Float,
             PinKind::Input, this, true}},
           {},
           "Client",
           {0, 255, 0}} {}