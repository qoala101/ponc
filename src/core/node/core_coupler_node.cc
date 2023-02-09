#include "core_coupler_node.h"
#include "core_empty_pin.h"
#include "core_float_pin.h"
#include "core_flow_pin.h"
#include "ui_coupler_node_drawer.h"

CouplerNode::CouplerNode(esc::IdGenerator& id_generator)
    : Node{id_generator.GetNext<ne::NodeId>(),
           {std::make_shared<FlowPin>(id_generator.GetNext<ne::PinId>(),
                                      PinKind::Input, false),
            std::make_shared<FloatPin>(id_generator.GetNext<ne::PinId>(), "",
                                       PinKind::Input, false),
            std::make_shared<FloatPin>(id_generator.GetNext<ne::PinId>(), "",
                                       PinKind::Input, false)},
           {std::make_shared<EmptyPin>(id_generator.GetNext<ne::PinId>(),
                                       PinKind::Output),
            std::make_shared<FlowPin>(id_generator.GetNext<ne::PinId>(),
                                      PinKind::Output, false),
            std::make_shared<FlowPin>(id_generator.GetNext<ne::PinId>(),
                                      PinKind::Output, false)}} {}

auto CouplerNode::GetDrawer() -> std::unique_ptr<vh::esc::ui::INodeDrawer> {
  return std::make_unique<vh::esc::ui::CouplerNodeDrawer>(shared_from_this());
}

auto CouplerNode::GetCouplerPercentageIndex() -> int& {
  return coupler_percentage_index_;
}

void CouplerNode::SetCouplerPercentageIndex(int index) {
  coupler_percentage_index_ = index;
}