#include "core_client_node.h"

#include <memory>

#include "core_float_pin.h"
#include "core_flow_pin.h"
#include "esc_cpp.h"
#include "esc_id_generator.h"

namespace esc {
ClientNode::ClientNode(esc::IdGenerator& id_generator)
    : Node{id_generator.GetNext<ne::NodeId>(),
           {std::make_shared<FlowPin>(id_generator.GetNext<ne::PinId>(),
                                      PinKind::Input, false),
            std::make_shared<FloatPin>(id_generator.GetNext<ne::PinId>(), "min",
                                       PinKind::Input, true),
            std::make_shared<FloatPin>(id_generator.GetNext<ne::PinId>(), "max",
                                       PinKind::Input, true)},
           {}} {}

auto ClientNode::CreateDrawer() -> std::unique_ptr<INodeDrawer> {
  return std::make_unique<ClientNodeDrawer>(shared_from_this());
}

ClientNodeDrawer::ClientNodeDrawer(std::shared_ptr<ClientNode> node)
    : node_{(cpp::Expects(node != nullptr), std::move(node))} {}

auto ClientNodeDrawer::GetLabel() const -> std::string { return "Client"; }

auto ClientNodeDrawer::GetColor() const -> ImColor { return {0, 255, 0}; }
}  // namespace esc