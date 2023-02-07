#include "esc_left_panel.h"

#include <imgui_node_editor.h>

#include <memory>
#include <utility>

#include "esc_app.h"
#include "esc_cpp.h"
#include "esc_nodes_and_links.h"
#include "esc_textures_handle.h"
#include "imgui.h"

namespace ne = ax::NodeEditor;

namespace esc {
namespace {
// vh: norm
void AddImage(ImTextureID texture_id, int alpha) {
  const auto p_min = ImGui::GetItemRectMin();
  const auto p_max = ImGui::GetItemRectMax();
  const auto uv_min = ImVec2{0, 0};
  const auto uv_max = ImVec2{1, 1};
  const auto col = IM_COL32(255, 255, 255, alpha);

  ImGui::GetWindowDrawList()->AddImage(texture_id, p_min, p_max, uv_min, uv_max,
                                       col);
}
}  // namespace

LeftPanel::LeftPanel(std::shared_ptr<App> app)
    : app_{(cpp::Expects(app != nullptr), std::move(app))}, file_browser_{[]() {
        auto file_browser_ = ImGui::FileBrowser{};
        file_browser_.SetTitle("Select");
        file_browser_.SetTypeFilters({".h", ".cpp"});
        return file_browser_;
      }()} {
  cpp::Ensures(app_ != nullptr);
}

void LeftPanel::Draw(float pane_width) {
  {
    const auto child_scope =
        cpp::Scope{[pane_width]() {
                     ImGui::BeginChild("Selection", ImVec2{pane_width, 0});
                   },
                   []() { ImGui::EndChild(); }};

    pane_width = ImGui::GetContentRegionAvail().x;

    {
      const auto horizontal_scope = cpp::Scope{
          [pane_width]() {
            ImGui::BeginHorizontal("Style Editor", ImVec2{pane_width, 0});
          },
          []() { ImGui::EndHorizontal(); }};

      if (ImGui::Button("Open...")) {
        file_browser_.Open();
      }

      if (ImGui::Button("Save As...")) {
        file_browser_.Open();
      }

      if (ImGui::Button("Zoom to Content")) {
        ne::NavigateToContent();
      }

      if (ImGui::Button("Show Flow")) {
        app_->ShowFlow();
      }
    }

    const auto selected_node_ids = esc::NodesAndLinks::GetSelectedNodeIds();
    const auto selected_link_ids = esc::NodesAndLinks::GetSelectedLinkIds();

    const auto texture_ids = app_->GetTextures().GetTextureIds();

    {
      const auto indent_scope =
          cpp::Scope{[]() { ImGui::Indent(); }, []() { ImGui::Unindent(); }};

      const auto& io = ImGui::GetIO();

      const auto save_icon_width =
          static_cast<float>(app_->GetTextureWidth(texture_ids.save_icon));
      const auto save_icon_height =
          static_cast<float>(app_->GetTextureHeight(texture_ids.save_icon));
      const auto restore_icon_width =
          static_cast<float>(app_->GetTextureWidth(texture_ids.restore_icon));
      const auto restore_icon_height =
          static_cast<float>(app_->GetTextureHeight(texture_ids.restore_icon));

      for (auto& node : app_->GetNodesAndLinks().GetNodes()) {
        {
          const auto id_scope =
              cpp::Scope{[&node]() { ImGui::PushID(node.ID.AsPointer()); },
                         []() { ImGui::PopID(); }};

          const auto cursor_pos = ImGui::GetCursorScreenPos();
          const auto node_label =
              node.Name + "##" +
              std::to_string(reinterpret_cast<uintptr_t>(node.ID.AsPointer()));
          auto is_node_selected =
              std::find(selected_node_ids.begin(), selected_node_ids.end(),
                        node.ID) != selected_node_ids.end();

          if (ImGui::Selectable(node_label.c_str(), &is_node_selected)) {
            if (io.KeyCtrl) {
              if (is_node_selected) {
                ne::SelectNode(node.ID, true);
              } else {
                ne::DeselectNode(node.ID);
              }
            } else {
              ne::SelectNode(node.ID, false);
            }

            ne::NavigateToSelection();
          }

          if (ImGui::IsItemHovered() && !node.State.empty()) {
            ImGui::SetTooltip("State: %s", node.State.c_str());
          }

          const auto& style = ImGui::GetStyle();
          const auto icon_panel_pos =
              cursor_pos +
              ImVec2{pane_width - style.FramePadding.x - style.IndentSpacing -
                         save_icon_width - restore_icon_width -
                         style.ItemInnerSpacing.x,
                     (ImGui::GetTextLineHeight() - save_icon_height) / 2};

          ImGui::SetCursorScreenPos(icon_panel_pos);
          ImGui::SetItemAllowOverlap();

          if (node.SavedState.empty()) {
            if (ImGui::InvisibleButton(
                    "save", ImVec2{save_icon_width, save_icon_height})) {
              node.SavedState = node.State;
            }

            if (ImGui::IsItemActive()) {
              AddImage(texture_ids.save_icon, 96);
            } else if (ImGui::IsItemHovered()) {
              AddImage(texture_ids.save_icon, 255);
            } else {
              AddImage(texture_ids.save_icon, 160);
            }
          } else {
            ImGui::Dummy(ImVec2{save_icon_width, save_icon_height});
            AddImage(texture_ids.save_icon, 32);
          }

          ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
          ImGui::SetItemAllowOverlap();

          if (!node.SavedState.empty()) {
            if (ImGui::InvisibleButton(
                    "restore",
                    ImVec2{restore_icon_width, restore_icon_height})) {
              node.State = node.SavedState;
              ne::RestoreNodeState(node.ID);
              node.SavedState.clear();
            }

            if (ImGui::IsItemActive()) {
              AddImage(texture_ids.restore_icon, 96);
            } else if (ImGui::IsItemHovered()) {
              AddImage(texture_ids.restore_icon, 255);
            } else {
              AddImage(texture_ids.restore_icon, 160);
            }
          } else {
            ImGui::Dummy(ImVec2{restore_icon_width, restore_icon_height});
            AddImage(texture_ids.restore_icon, 32);
          }

          ImGui::SameLine(0, 0);
          ImGui::SetItemAllowOverlap();

          ImGui::Dummy(ImVec2{0, restore_icon_height});
        }
      }
    }
  }

  file_browser_.Display();

  if (file_browser_.HasSelected()) {
    const auto selected_file_path = file_browser_.GetSelected().string();

    file_browser_.ClearSelected();
  }
}

}  // namespace esc