#include "core_splitter_node.h"

#include "core_float_pin.h"
#include "core_flow_pin.h"

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
           }(),
           "Splitter 1x" + std::to_string(n),
           {0, 0, 127 + 128 / n}} {}