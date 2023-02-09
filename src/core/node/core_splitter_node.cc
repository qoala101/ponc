  #include "core_splitter_node.h"
  
  SplitterNode::SplitterNode(esc::IdGenerator& id_generator, int n)
      : Node{id_generator.GetNext<ne::NodeId>(),
             "Splitter 1x" + std::to_string(n),
             {0, 0, 127 + 128 / n}} {
    Inputs.emplace_back(id_generator.GetNext<ne::PinId>(), "", PinType::Flow,
                        PinKind::Input, this);
    Inputs.emplace_back(id_generator.GetNext<ne::PinId>(), "", PinType::Float,
                        PinKind::Input, this);

    for (auto i = 0; i < n; ++i) {
      Outputs.emplace_back(id_generator.GetNext<ne::PinId>(), "", PinType::Flow,
                           PinKind::Output, this);
    }
  }