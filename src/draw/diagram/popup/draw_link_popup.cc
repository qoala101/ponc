#include "draw_link_popup.h"

#include "draw_native_facade.h"
#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
///
auto GetTitle(const std::vector<ne::LinkId>& links) {
  if (links.empty()) {
    return std::string{};
  }

  const auto num_links = links.size();

  if (num_links == 1) {
    return "Link #" + std::to_string(links.front().Get());
  }

  return "Links (" + std::to_string(links.size()) + ")";
}
}  // namespace

///
void LinkPopup::Draw(const Callbacks& callbacks) {
  const auto selected_links =
      IsOpened() ? NativeFacade::GetSelectedLinks() : std::vector<ne::LinkId>{};
  const auto title = GetTitle(selected_links);
  const auto content_scope = DrawContentScope(title);

  if (!IsOpened()) {
    return;
  }

  if (ImGui::MenuItem("Delete")) {
    callbacks.delete_selected(selected_links);
  }
}
}  // namespace esc::draw