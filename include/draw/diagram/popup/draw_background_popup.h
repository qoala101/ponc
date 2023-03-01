#ifndef VH_DRAW_BACKGROUND_POPUP_H_
#define VH_DRAW_BACKGROUND_POPUP_H_

#include <imgui.h>

#include <memory>

#include "core_i_family.h"
#include "coreui_project.h"
#include "draw_i_popup.h"

namespace esc::draw {
class BackgroundPopup : public IPopup {
 public:
  using SignalCreateNode = std::function<void(
      const std::shared_ptr<core::IFamily> &family, const ImVec2 &pos)>;

  void Draw(const std::vector<std::shared_ptr<core::IFamily>> &families,
            const SignalCreateNode &signal_create_node);
  void SetPosition(const ImVec2 &position);

 private:
  auto GetLabel() const -> std::string override;

  ImVec2 position_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_BACKGROUND_POPUP_H_
