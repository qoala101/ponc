#ifndef VH_DRAW_BACKGROUND_CONNECTION_POPUP_H_
#define VH_DRAW_BACKGROUND_CONNECTION_POPUP_H_

#include <imgui.h>
#include <imgui_node_editor.h>

#include <memory>

#include "core_i_node.h"
#include "coreui_family.h"
#include "cpp_callbacks.h"
#include "draw_i_popup.h"

namespace esc::draw {
///
class CreateNodePopup : public IPopup {
 public:
  ///
  struct Callbacks {
    ///
    std::optional<cpp::Signal<>> closed{};
    ///
    cpp::Signal<std::unique_ptr<core::INode>> node_created{};
  };

  ///
  void Draw(const std::vector<coreui::FamilyGroup> &family_groups,
            const Callbacks &callbacks);

 private:
  ///
  auto GetLabel() const -> std::string override;
};
}  // namespace esc::draw

#endif  // VH_DRAW_BACKGROUND_CONNECTION_POPUP_H_
