#include "draw_question_dialog.h"

#include "cpp_assert.h"
#include "imgui.h"

namespace vh::ponc::draw {
///
QuestionDialog::QuestionDialog(Labels labels) : labels_{std::move(labels)} {}

///
void QuestionDialog::Open() { open_requested_ = true; }

///
void QuestionDialog::Draw(const Callbacks &callbacks) {
  const auto *title_data = labels_.title.c_str();

  if (open_requested_) {
    ImGui::OpenPopup(title_data);
    open_requested_ = false;
  }

  const auto *viewport = ImGui::GetMainViewport();
  Expects(viewport != nullptr);

  ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing,
                          ImVec2{0.5, 0.5});

  if (ImGui::BeginPopupModal(title_data, nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::TextUnformatted(labels_.question.data());
    ImGui::Separator();
    ImGui::BeginHorizontal("Buttons");

    if (ImGui::Button(labels_.accept.c_str())) {
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
}  // namespace vh::ponc::draw