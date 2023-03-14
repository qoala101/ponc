#include "draw_new_project_dialog.h"

#include "cpp_assert.h"
#include "imgui.h"

namespace esc::draw {
namespace {
///
constexpr auto kTitle = "New Project";
}  // namespace

///
void NewProjectDialog::Open() { open_requested_ = true; }

///
void NewProjectDialog::Draw(const Callbacks &callbacks) {
  if (open_requested_) {
    ImGui::OpenPopup(kTitle);
    open_requested_ = false;
  }

  const auto *viewport = ImGui::GetMainViewport();
  Expects(viewport != nullptr);

  ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing,
                          ImVec2{0.5F, 0.5F});

  if (ImGui::BeginPopupModal(kTitle, nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::TextUnformatted("Current project would not be saved.\nProceed?");
    ImGui::Separator();
    ImGui::BeginHorizontal("Buttons");

    if (ImGui::Button("New Project")) {
      callbacks.accepted();
      ImGui::CloseCurrentPopup();
    }

    if (ImGui::Button("Cancel")) {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndHorizontal();
    ImGui::EndPopup();
  }
}
}  // namespace esc::draw