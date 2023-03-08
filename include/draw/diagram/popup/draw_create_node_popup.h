#ifndef VH_DRAW_CREATE_NODE_POPUP_H_
#define VH_DRAW_CREATE_NODE_POPUP_H_

#include <imgui.h>
#include <imgui_node_editor.h>

#include <memory>

#include "core_i_family.h"
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
    std::optional<cpp::Query<bool, const core::IFamily &>> is_family_enabled{};
    ///
    std::optional<cpp::Signal<>> closed{};
    ///
    cpp::Signal<std::unique_ptr<core::INode>> node_created{};
  };

  ///
  void Draw(const std::vector<coreui::FamilyGroup> &family_groups,
            const Callbacks &callbacks);
  ///
  void SetPos(const ImVec2 &pos);

 private:
  ///
  auto GetLabel() const -> std::string override;

  ///
  ImVec2 pos_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_CREATE_NODE_POPUP_H_
