/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_NODE_VIEW_H_
#define VH_PONC_DRAW_NODE_VIEW_H_

#include <string>

#include "coreui_family.h"
#include "draw_i_view.h"

namespace vh::ponc::draw {
///
class NodeView : public IView {
 public:
  ///
  auto GetLabel() const -> std::string override;

  ///
  void Draw(std::optional<const coreui::Node *> node,
            const std::vector<coreui::FamilyGroup> &family_groups);
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_NODE_VIEW_H_
