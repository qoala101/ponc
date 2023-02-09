#include "core_input_node.h"

  InputNode::InputNode(esc::IdGenerator& id_generator)
      : Node{id_generator.GetNext<ne::NodeId>(), "Input", {255, 0, 0}} {
    auto& pin = Outputs.emplace_back(id_generator.GetNext<ne::PinId>(), "",
                                     PinType::Flow, PinKind::Output, this);
    pin.editable = true;
  }