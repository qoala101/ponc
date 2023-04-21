#ifndef VH_PONC_DRAW_CALCULATOR_VIEW_H_
#define VH_PONC_DRAW_CALCULATOR_VIEW_H_

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "calc_calculation_task.h"
#include "calc_settings.h"
#include "core_project.h"
#include "cpp_callbacks.h"
#include "draw_i_view.h"

namespace vh::ponc::draw {
///
class CalculatorView : public IView {
 public:
  ///
  struct Callbacks {
    ///
    cpp::Signal<std::vector<core::Diagram>> calculated_diagrams{};
  };

  ///
  auto GetLabel() const -> std::string override;
  ///
  void Draw(core::Project &project, const Callbacks &callbacks);

 private:
  ///
  void DrawProgressBar(const calc::CalculatorSettings &settings);

  ///
  std::optional<calc::CalculationTask> calculation_task_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_CALCULATOR_VIEW_H_
