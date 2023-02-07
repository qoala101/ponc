#include "esc_left_pane.h"

#include <bits/ranges_algo.h>
#include <imgui_node_editor.h>

#include <memory>
#include <utility>

#include "esc_app.h"
#include "esc_cpp.h"
#include "esc_nodes_and_links.h"
#include "imgui.h"

namespace ne = ax::NodeEditor;

namespace esc {
namespace {
// vh: norm
void DrawNode(Node& node, bool is_selected) {
  const auto& io = ImGui::GetIO();

  {
    const auto id_scope =
        cpp::Scope{[&node]() { ImGui::PushID(node.ID.AsPointer()); },
                   []() { ImGui::PopID(); }};

    const auto label =
        node.Name + "##" +
        std::to_string(reinterpret_cast<uintptr_t>(node.ID.AsPointer()));

    if (ImGui::Selectable(label.c_str(), &is_selected)) {
      if (io.KeyCtrl) {
        if (is_selected) {
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
  }
}
}  // namespace
// vh: norm
LeftPane::LeftPane(std::shared_ptr<App> app)
    : app_{(cpp::Expects(app != nullptr), std::move(app))}, file_browser_{[]() {
        auto file_browser_ = ImGui::FileBrowser{};
        file_browser_.SetTitle("Select");
        file_browser_.SetTypeFilters({".h", ".cpp"});
        return file_browser_;
      }()} {
  cpp::Ensures(app_ != nullptr);
}
// vh: norm
void LeftPane::Draw(float pane_width) {
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

      DrawMenu();
    }

    const auto selected_node_ids = esc::NodesAndLinks::GetSelectedNodeIds();

    {
      const auto indent_scope =
          cpp::Scope{[]() { ImGui::Indent(); }, []() { ImGui::Unindent(); }};

      for (auto& node : app_->GetNodesAndLinks().GetNodes()) {
        const auto is_node_selected =
            std::ranges::find(selected_node_ids, node.ID) !=
            selected_node_ids.end();

        DrawNode(node, is_node_selected);
      }
    }
  }

  DrawDialog();
}
// vh: norm
void LeftPane::DrawMenu() {
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
// vh: norm
void LeftPane::DrawDialog() {
  file_browser_.Display();

  if (file_browser_.HasSelected()) {
    const auto selected_file_path = file_browser_.GetSelected().string();

    file_browser_.ClearSelected();
  }
}
}  // namespace esc