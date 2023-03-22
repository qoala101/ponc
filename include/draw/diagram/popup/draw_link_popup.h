#ifndef VH_PONC_DRAW_LINK_POPUP_H_
#define VH_PONC_DRAW_LINK_POPUP_H_

#include <imgui_node_editor.h>

#include "cpp_callbacks.h"
#include "draw_i_popup.h"

namespace ne = ax::NodeEditor;

namespace vh::ponc::draw {
///
class LinkPopup : public IPopup {
 public:
  ///
  struct Callbacks {
    ///
    cpp::Signal<std::vector<ne::LinkId>> delete_selected{};
  };

  ///
  void Draw(const Callbacks &callbacks);
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_LINK_POPUP_H_
