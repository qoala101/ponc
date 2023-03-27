#include "flow_calculator.h"

#include <vector>

#include "core_id_generator.h"
#include "flow_tree.h"
#include "imgui_node_editor.h"

namespace vh::ponc::flow {
auto Calculate(const CalculatorInput &input) -> std::vector<Diagram> {
  auto id_generator = core::IdGenerator{};

  auto result =
      Diagram{.flow_tree = FlowTree{
                  .root_nodes = {TreeNode{
                      .node_id = id_generator.Generate<ne::NodeId>()}}}};

  result.node_families.emplace(result.flow_tree.root_nodes.front().node_id,
                               input.family_flows.begin()->first);

  return {result};
}
}  // namespace vh::ponc::flow