#include "core_coupler_node.h"

CouplerNode::CouplerNode(esc::IdGenerator& id_generator)
    : Node{id_generator.GetNext<ne::NodeId>(),
           {{id_generator.GetNext<ne::PinId>(), "", PinType::Flow,
             PinKind::Input, false},
            {id_generator.GetNext<ne::PinId>(), "", PinType::Float,
             PinKind::Input, false},
            {id_generator.GetNext<ne::PinId>(), "", PinType::Float,
             PinKind::Input, false}},
           {{id_generator.GetNext<ne::PinId>(), "", PinType::Empty,
             PinKind::Output, false},
            {id_generator.GetNext<ne::PinId>(), "", PinType::Flow,
             PinKind::Output, false},
            {id_generator.GetNext<ne::PinId>(), "", PinType::Flow,
             PinKind::Output, false}},
           "Coupler 1x2",
           {255, 0, 255}} {}

auto CouplerNode::GetCouplerPercentageIndex() -> int& {
  return coupler_percentage_index_;
}

void CouplerNode::SetCouplerPercentageIndex(int index) {
  coupler_percentage_index_ = index;
}