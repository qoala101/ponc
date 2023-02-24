/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_DRAW_LINK_POPUP_H_
#define VH_DRAW_LINK_POPUP_H_

#include <imgui_node_editor.h>

#include "draw_i_popup.h"

namespace ne = ax::NodeEditor;

namespace esc::draw {
class LinkPopup : public IPopup {
 public:
  void SetLinkId(ne::LinkId link_id);

 private:
  auto GetLabel() const -> std::string override;
  void DrawItems(const AppState &app_state) override;

  ne::LinkId link_id_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_LINK_POPUP_H_
