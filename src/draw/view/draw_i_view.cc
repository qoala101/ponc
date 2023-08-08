/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_i_view.h"

#include <imgui.h>

#include <functional>

namespace vh::ponc::draw {
///
auto IView::IsOpened() const -> bool { return opened_; }

///
void IView::SetOpened(bool opened) { opened_ = opened; }

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