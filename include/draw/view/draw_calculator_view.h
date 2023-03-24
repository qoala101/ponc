#ifndef VH_PONC_DRAW_CALCULATOR_VIEW_H_
#define VH_PONC_DRAW_CALCULATOR_VIEW_H_

#include <string>

#include "coreui_diagram.h"
#include "coreui_family.h"
#include "draw_i_view.h"
#include "flow_calculator.h"

namespace vh::ponc::draw {
class CalculatorView : public IView {
 public:
  auto GetLabel() const -> std::string override;

  void Draw(const coreui::Diagram &diagram,
            std::optional<const coreui::Node *> node);

 private:
  std::optional<flow::CalculatorInput> calculator_input_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_CALCULATOR_VIEW_H_
