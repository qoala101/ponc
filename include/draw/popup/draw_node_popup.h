#ifndef VH_DRAW_NODE_POPUP_H_
#define VH_DRAW_NODE_POPUP_H_

#include "draw_i_popup.h"
#include "imgui_node_editor.h"

namespace esc::draw {
// ---
class NodePopup : public IPopup {
 public:
  explicit NodePopup(std::shared_ptr<core::INode> node);

 private:
  // ---
  auto GetLabel [[nodiscard]] (State &state) const -> std::string override;
  // ---
  void DrawContent(State &state) override;

  std::shared_ptr<core::INode> node_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_NODE_POPUP_H_
