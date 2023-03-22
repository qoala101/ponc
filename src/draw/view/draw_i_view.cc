#include "draw_i_view.h"

#include <imgui.h>

namespace vh::ponc::draw {
///
auto IView::IsOpened() const -> bool { return opened_; }

///
void IView::Toggle() { opened_ = !opened_; }

///
auto IView::DrawContentScope() -> cpp::ScopeFunction {
  if (!opened_) {
    return cpp::ScopeFunction{};
  }

  ImGui::Begin(GetLabel().c_str(), &opened_);

  if (ImGui::BeginPopupContextWindow()) {
    if (ImGui::MenuItem("Close")) {
      opened_ = false;
    }

    ImGui::EndPopup();
  }

  return cpp::ScopeFunction{[]() { ImGui::End(); }};
}
}  // namespace vh::ponc::draw