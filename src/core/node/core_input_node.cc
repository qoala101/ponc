#include "core_input_node.h"

#include "core_flow_pin.h"
#include "ui_input_node_drawer.h"

InputNode::InputNode(esc::IdGenerator& id_generator)
    : Node{id_generator.GetNext<ne::NodeId>(),
           {},
           {std::make_shared<FlowPin>(id_generator.GetNext<ne::PinId>(),
                                      PinKind::Output, true)}} {}

auto InputNode::GetDrawer() -> std::unique_ptr<vh::esc::ui::INodeDrawer> {
  return std::make_unique<vh::esc::ui::InputNodeDrawer>(shared_from_this());
}