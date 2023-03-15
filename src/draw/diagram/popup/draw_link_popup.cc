#include "draw_link_popup.h"

#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
///
auto GetSelectedLinks() {
  auto selected_links = std::vector<ne::LinkId>{};
  selected_links.resize(ne::GetSelectedObjectCount());

  const auto num_selected_links = ne::GetSelectedLinks(
      selected_links.data(), static_cast<int>(selected_links.size()));
  selected_links.resize(num_selected_links);

  return selected_links;
}

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
  const auto selected_nodes =
      IsOpened() ? GetSelectedLinks() : std::vector<ne::LinkId>{};
  const auto title = GetTitle(selected_nodes);
  const auto content_scope = DrawContentScope(title);

  if (!IsOpened()) {
    return;
  }

  if (ImGui::MenuItem("Delete")) {
    callbacks.delete_selected(GetSelectedLinks());
  }
}
}  // namespace esc::draw