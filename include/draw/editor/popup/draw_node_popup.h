/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_DRAW_NODE_POPUP_H_
#define VH_DRAW_NODE_POPUP_H_

#include <imgui_node_editor.h>

#include "draw_i_popup.h"

namespace ne = ax::NodeEditor;

namespace esc::draw {
// ---
class NodePopup : public IPopup {
 public:
  // ---
  void SetNodeId(ne::NodeId node_id);

 private:
  // ---
  auto GetLabel [[nodiscard]] () const -> std::string override;
  // ---
  void DrawItems(const AppState &app_state) override;

  // ---
  ne::NodeId node_id_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_NODE_POPUP_H_
