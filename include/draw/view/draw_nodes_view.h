#ifndef VH_DRAW_NODES_VIEW_H_
#define VH_DRAW_NODES_VIEW_H_

#include <string>

#include "coreui_diagram.h"
#include "draw_i_view.h"

namespace esc::draw {
///
class NodesView : public IView {
 public:
  ///
  auto GetLabel() const -> std::string override;

  ///
  void Draw(const coreui::Diagram &diagram);
};
}  // namespace esc::draw

#endif  // VH_DRAW_NODES_VIEW_H_