/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_CALCULATOR_VIEW_H_
#define VH_PONC_DRAW_CALCULATOR_VIEW_H_

#include <string>

#include "core_project.h"
#include "coreui_calculator.h"
#include "draw_i_view.h"

namespace vh::ponc::draw {
///
class CalculatorView : public IView {
 public:
  ///
  auto GetLabel() const -> std::string override;

  ///
  void Draw(coreui::Calculator& calculator, core::Project& project);
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_CALCULATOR_VIEW_H_
