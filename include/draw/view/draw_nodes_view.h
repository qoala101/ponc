#ifndef VH_PONC_DRAW_NODES_VIEW_H_
#define VH_PONC_DRAW_NODES_VIEW_H_

#include <string>

#include "coreui_diagram.h"
#include "draw_i_view.h"

namespace vh::ponc::draw {
///
class NodesView : public IView {
 public:
  ///
  auto GetLabel() const -> std::string override;

  ///
  void Draw(const coreui::Diagram &diagram);
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_NODES_VIEW_H_
