#include "core_input_node.h"

#include "core_flow_pin.h"
#include "esc_cpp.h"

namespace esc {
InputNode::InputNode(esc::IdGenerator& id_generator)
    : Node{id_generator.GetNext<ne::NodeId>(),
           {},
           {std::make_shared<FlowPin>(id_generator.GetNext<ne::PinId>(),
                                      PinKind::Output, true)}} {}

auto InputNode::CreateDrawer() -> std::unique_ptr<INodeDrawer> {
  return std::make_unique<InputNodeDrawer>(shared_from_this());
}

InputNodeDrawer::InputNodeDrawer(std::shared_ptr<InputNode> node)
    : node_{(cpp::Expects(node != nullptr), std::move(node))} {}

auto InputNodeDrawer::GetLabel() const -> std::string { return "Input"; }

auto InputNodeDrawer::GetColor() const -> ImColor { return {255, 0, 255}; }
}  // namespace esc