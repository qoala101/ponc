#ifndef VH_PONC_DRAW_CALCULATOR_VIEW_H_
#define VH_PONC_DRAW_CALCULATOR_VIEW_H_

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "calc_calculation_task.h"
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
  void DrawProgressBar() const;

  ///
  std::optional<calc::CalculationTask> calculation_task_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_CALCULATOR_VIEW_H_
