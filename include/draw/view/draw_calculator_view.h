#ifndef VH_PONC_DRAW_CALCULATOR_VIEW_H_
#define VH_PONC_DRAW_CALCULATOR_VIEW_H_

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "calc_calculation_task.h"
#include "calc_tree_node.h"
#include "core_diagram.h"
#include "coreui_project.h"
#include "draw_i_view.h"

namespace vh::ponc::draw {
///
class CalculatorView : public IView {
 public:
  ///
  auto GetLabel() const -> std::string override;
  ///
  void Draw(coreui::Project& project);

 private:
  ///
  void PopulateOutput(const calc::TreeNode& output_tree, ne::PinId output_pin,
                      core::Project& project);
  ///
  void PopulateDiagram(const calc::TreeNode& calculated_tree,
                       core::Project& project);
  ///
  void ProcessResult(coreui::Project& project);
  ///
  void DrawProgressBar() const;

  ///
  std::optional<core::Diagram> diagram_copy_{};
  ///
  std::optional<calc::CalculationTask> calculation_task_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_CALCULATOR_VIEW_H_
