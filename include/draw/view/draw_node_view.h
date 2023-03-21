#ifndef VH_DRAW_NODE_VIEW_H_
#define VH_DRAW_NODE_VIEW_H_

#include <string>

#include "coreui_family.h"
#include "draw_i_view.h"

namespace esc::draw {
///
class NodeView : public IView {
 public:
  ///
  auto GetLabel() const -> std::string override;

  ///
  void Draw(std::optional<const coreui::Node *> node,
            const std::vector<coreui::FamilyGroup> &family_groups);
};
}  // namespace esc::draw

#endif  // VH_DRAW_NODE_VIEW_H_
