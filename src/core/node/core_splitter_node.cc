#include "core_splitter_node.h"

#include "core_float_pin.h"
#include "core_flow_pin.h"
#include "esc_cpp.h"

namespace esc {
SplitterNode::SplitterNode(esc::IdGenerator& id_generator, int n)
    : Node{id_generator.GetNext<ne::NodeId>(),
           {std::make_shared<FlowPin>(id_generator.GetNext<ne::PinId>(),
                                      PinKind::Input, false),
            std::make_shared<FloatPin>(id_generator.GetNext<ne::PinId>(), "",
                                       PinKind::Input, false)},
           [&id_generator, n]() {
             auto pins = std::vector<std::shared_ptr<Pin>>{};

             for (auto i = 0; i < n; ++i) {
               pins.emplace_back(std::make_shared<FlowPin>(
                   id_generator.GetNext<ne::PinId>(), PinKind::Output, false));
             }

             return pins;
           }()} {}

auto SplitterNode::CreateDrawer() -> std::unique_ptr<INodeDrawer> {
  return std::make_unique<SplitterNodeDrawer>(shared_from_this());
}

SplitterNodeDrawer::SplitterNodeDrawer(std::shared_ptr<SplitterNode> node)
    : node_{(cpp::Expects(node != nullptr), std::move(node))} {}

auto SplitterNodeDrawer::GetNumOutputs() const {
  return static_cast<int>(node_->GetOutputPins().size());
}

auto SplitterNodeDrawer::GetLabel() const -> std::string {
  return "Splitter 1x" + std::to_string(GetNumOutputs());
}

auto SplitterNodeDrawer::GetColor() const -> ImColor {
  return {0, 0, 127 + 128 / GetNumOutputs()};
}
}  // namespace esc