#include "core_coupler_node.h"

  CouplerNode::CouplerNode(esc::IdGenerator& id_generator)
      : Node{id_generator.GetNext<ne::NodeId>(), "Coupler 1x2", {255, 0, 255}} {
    Inputs.emplace_back(id_generator.GetNext<ne::PinId>(), "", PinType::Flow,
                        PinKind::Input, this);
    Inputs.emplace_back(id_generator.GetNext<ne::PinId>(), "", PinType::Float,
                        PinKind::Input, this);
    Inputs.emplace_back(id_generator.GetNext<ne::PinId>(), "", PinType::Float,
                        PinKind::Input, this);

    Outputs.emplace_back(id_generator.GetNext<ne::PinId>(), "", PinType::Empty,
                         PinKind::Output, this);
    Outputs.emplace_back(id_generator.GetNext<ne::PinId>(), "", PinType::Flow,
                         PinKind::Output, this);
    Outputs.emplace_back(id_generator.GetNext<ne::PinId>(), "", PinType::Flow,
                         PinKind::Output, this);
  }