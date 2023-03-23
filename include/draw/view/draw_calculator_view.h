#ifndef VH_PONC_DRAW_CALCULATOR_VIEW_H_
#define VH_PONC_DRAW_CALCULATOR_VIEW_H_

#include <string>

#include "coreui_family.h"
#include "draw_i_view.h"

namespace vh::ponc::draw {
///
class CalculatorView : public IView {
 public:
  ///
  auto GetLabel() const -> std::string override;

  ///
  void Draw(std::optional<const coreui::Node *> node,
            const std::vector<coreui::FamilyGroup> &family_groups);
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_CALCULATOR_VIEW_H_
