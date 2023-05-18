/**
 * PON Calculator https://github.com/qoala101/ponc
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_link_popup.h"

#include "coreui_native_facade.h"
#include "imgui_node_editor.h"

namespace vh::ponc::draw {
namespace {
///
auto GetTitle(const std::vector<ne::LinkId>& links) {
  if (links.empty()) {
    return std::string{};
  }

  if (links.size() == 1) {
    return "Link #" + std::to_string(links.front().Get());
  }

  return "Links (" + std::to_string(links.size()) + ")";
}
}  // namespace

///
void LinkPopup::Draw(const Callbacks& callbacks) {
  const auto selected_links = IsOpened()
                                  ? coreui::NativeFacade::GetSelectedLinks()
                                  : std::vector<ne::LinkId>{};
  const auto title = GetTitle(selected_links);
  const auto content_scope = DrawContentScope(title);

  if (!IsOpened()) {
    return;
  }

  if (ImGui::MenuItem("Delete")) {
    callbacks.delete_selected(selected_links);
  }
}
}  // namespace vh::ponc::draw