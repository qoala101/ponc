#ifndef VH_DRAW_LINK_POPUP_H_
#define VH_DRAW_LINK_POPUP_H_

#include <imgui_node_editor.h>

#include "cpp_callbacks.h"
#include "draw_i_popup.h"

namespace ne = ax::NodeEditor;

namespace esc::draw {
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
}  // namespace esc::draw

#endif  // VH_DRAW_LINK_POPUP_H_
