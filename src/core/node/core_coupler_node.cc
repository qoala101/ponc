#include "core_coupler_node.h"

#include "core_empty_pin.h"
#include "core_float_pin.h"
#include "core_flow_pin.h"
#include "esc_cpp.h"

namespace esc {
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

auto CouplerNode::CreateDrawer() -> std::unique_ptr<INodeDrawer> {
  return std::make_unique<CouplerNodeDrawer>(shared_from_this());
}

auto CouplerNode::GetCouplerPercentageIndex() -> int& {
  return coupler_percentage_index_;
}

void CouplerNode::SetCouplerPercentageIndex(int index) {
  coupler_percentage_index_ = index;
}

CouplerNodeDrawer::CouplerNodeDrawer(std::shared_ptr<CouplerNode> node)
    : node_{(cpp::Expects(node != nullptr), std::move(node))} {}

auto CouplerNodeDrawer::GetLabel() const -> std::string {
  return "Coupler 1x2";
}

auto CouplerNodeDrawer::GetColor() const -> ImColor { return {255, 0, 255}; }
}  // namespace esc