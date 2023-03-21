#include "cpp_assert.h"
#include "draw_question_dialog.h"
#include "imgui.h"

namespace esc::draw {
QuestionDialog::QuestionDialog(const ConstructorArgs &args)
    : title_{args.title}, question_{args.question}, ok_label_{args.ok_label} {}

///
void QuestionDialog::Open() { open_requested_ = true; }

///
void QuestionDialog::Draw(const Callbacks &callbacks) {
  const auto *title_data = title_.c_str();

  if (open_requested_) {
    ImGui::OpenPopup(title_data);
    open_requested_ = false;
  }

  const auto *viewport = ImGui::GetMainViewport();
  Expects(viewport != nullptr);

  ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing,
                          ImVec2{0.5F, 0.5F});

  if (ImGui::BeginPopupModal(title_data, nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::TextUnformatted(question_.data());
    ImGui::Separator();
    ImGui::BeginHorizontal("Buttons");

    if (ImGui::Button(ok_label_.c_str())) {
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