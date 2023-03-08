#include "draw_i_popup.h"

#include <imgui.h>
#include <imgui_node_editor.h>

#include "cpp_scope_function.h"
#include "draw_id_label.h"

namespace ne = ax::NodeEditor;

namespace esc::draw {
namespace {
///
auto GenerateId() {
  static auto id = uintptr_t{};
  return id++;
}
}  // namespace

///
void IPopup::Open() {
  ImGui::OpenPopup(IdLabel(GetLabel(), id_).c_str(),
                   ImGuiPopupFlags_NoOpenOverExistingPopup);
  opened_ = true;
}

//
auto IPopup::IsOpened() const -> bool { return opened_; }

///
IPopup::IPopup() : id_{GenerateId()} {}

///
auto IPopup::DrawContentScope(const Callbacks &callbacks)
    -> cpp::ScopeFunction {
  const auto label = GetLabel();
  const auto id_label = IdLabel(label, id_);

  ne::Suspend();

  if (ImGui::BeginPopup(id_label.c_str())) {
    ImGui::TextUnformatted(label.c_str());
    ImGui::Separator();

    return cpp::ScopeFunction{[]() {
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
}  // namespace esc::draw