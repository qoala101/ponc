#ifndef VH_PONC_DRAW_CALCULATOR_VIEW_H_
#define VH_PONC_DRAW_CALCULATOR_VIEW_H_

#include <string>
#include <vector>

#include "coreui_diagram.h"
#include "coreui_family.h"
#include "cpp_callbacks.h"
#include "draw_i_view.h"
#include "flow_calculator.h"
#include "flow_tree.h"

namespace vh::ponc::draw {
class CalculatorView : public IView {
 public:
  struct Callbacks {
    cpp::Signal<std::vector<core::Diagram>> calculated_diagrams{};
  };

  auto GetLabel() const -> std::string override;

  void Draw(const coreui::Diagram &diagram,
            std::optional<const coreui::Node *> node,
            const Callbacks &callbacks);

 private:
  auto GetNodeInputRange(ne::NodeId) -> auto &;
  void DrawMinNodeInput(const coreui::TreeNode &tree_node);
  void DrawMaxNodeInput(const coreui::TreeNode &tree_node);

  flow::TreeNode tree_node_{};
  std::unordered_map<uintptr_t, int> family_costs_{};
  std::unordered_map<uintptr_t, flow::InputRange> required_inputs_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_CALCULATOR_VIEW_H_
