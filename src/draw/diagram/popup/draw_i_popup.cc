/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_i_popup.h"

#include <imgui.h>
#include <imgui_node_editor.h>

#include <functional>
#include <string>

#include "cpp_scope_function.h"

namespace ne = ax::NodeEditor;

namespace vh::ponc::draw {
namespace {
///
auto GenerateIdLabel() {
  static auto id = ImGuiID{};
  return std::string{"##"} + std::to_string(id++);
}
}  // namespace

//
auto IPopup::IsOpened() const -> bool { return opened_; }

///
void IPopup::Open() {
  ImGui::OpenPopup(id_label_.c_str(), ImGuiPopupFlags_NoOpenOverExistingPopup);

  const auto was_opened = opened_;
  opened_ = true;

  if (!was_opened) {
    just_opened_ = true;
  }
}

///
IPopup::IPopup() : id_label_{GenerateIdLabel()} {}

///
void IPopup::OnOpen() const {}

///
auto IPopup::WasJustOpened() const -> bool { return just_opened_; }

///
auto IPopup::DrawContentScope(std::string_view title,
                              const Callbacks &callbacks)
    -> cpp::ScopeFunction {
  ne::Suspend();

  if (just_opened_) {
    OnOpen();
  }

  if (ImGui::BeginPopup(id_label_.c_str())) {
    if (!title.empty()) {
      ImGui::TextUnformatted(title.data());
      ImGui::Separator();
    }

    return cpp::ScopeFunction{
        [just_opened = safe_owner_.MakeSafe(&just_opened_)]() {
          *just_opened = false;

          ImGui::EndPopup();
          ne::Resume();
        }};
  }

  if (opened_) {
    opened_ = false;

    if (callbacks.closed.has_value()) {
      (*callbacks.closed)();
    }
  }

  return cpp::ScopeFunction{[]() { ne::Resume(); }};
}
}  // namespace vh::ponc::draw