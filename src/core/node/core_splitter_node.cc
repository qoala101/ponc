#include "core_splitter_node.h"

SplitterNode::SplitterNode(esc::IdGenerator& id_generator, int n)
    : Node{id_generator.GetNext<ne::NodeId>(),
           {{id_generator.GetNext<ne::PinId>(), "", PinType::Flow,
             PinKind::Input, this, false},
            {id_generator.GetNext<ne::PinId>(), "", PinType::Float,
             PinKind::Input, this, false}},
           [&id_generator, n, this]() {
             auto pins = std::vector<Pin>{};

             for (auto i = 0; i < n; ++i) {
               pins.emplace_back(id_generator.GetNext<ne::PinId>(), "",
                                 PinType::Flow, PinKind::Output, this, false);
             }

             return pins;
           }(),
           "Splitter 1x" + std::to_string(n),
           {0, 0, 127 + 128 / n}} {}