#ifndef VH_PONC_DRAW_DIAGRAMS_VIEW_H_
#define VH_PONC_DRAW_DIAGRAMS_VIEW_H_

#include <string>

#include "coreui_project.h"
#include "draw_i_view.h"

namespace vh::ponc::draw {
///
class DiagramsView : public IView {
 public:
  ///
  auto GetLabel() const -> std::string override;

  ///
  void Draw(coreui::Project &project);
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_DIAGRAMS_VIEW_H_
