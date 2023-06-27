/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_rename_widget.h"

#include "draw_disable_if.h"

namespace vh::ponc::draw {
///
auto DrawRenameWidget(std::string_view label, StringBuffer &name_buffer)
    -> bool {
  auto rename_confirmed = false;

  const auto name_is_empty = name_buffer.AsTrimmed().empty();

  if (ImGui::InputText(label.data(), name_buffer.GetData(),
                       name_buffer.GetSize(),
                       ImGuiInputTextFlags_AutoSelectAll |
                           ImGuiInputTextFlags_EnterReturnsTrue)) {
    if (!name_is_empty) {
      rename_confirmed = true;
    } else {
      ImGui::SetKeyboardFocusHere(-1);
    }
  }

  ImGui::SameLine();

  {
    const auto disable_scope = DisableIf(name_is_empty);

    if (ImGui::Button("OK")) {
      rename_confirmed = true;
    }
  }

  if (rename_confirmed) {
    ImGui::CloseCurrentPopup();
  }

  return rename_confirmed;
}
}  // namespace vh::ponc::draw