#include "esc_left_pane.h"

#include <bits/ranges_algo.h>
#include <imgui_node_editor.h>

#include <iostream>
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
        cpp::Scope{[&node]() { ImGui::PushID(node.GetId().AsPointer()); },
                   []() { ImGui::PopID(); }};

    const auto label = node.GetDrawer()->GetName();

    if (ImGui::Selectable(label.c_str(), &is_selected)) {
      if (io.KeyCtrl) {
        if (is_selected) {
          ne::SelectNode(node.GetId(), true);
        } else {
          ne::DeselectNode(node.GetId());
        }
      } else {
        ne::SelectNode(node.GetId(), false);
      }

      ne::NavigateToSelection();
    }
  }
}

auto AsLowerCase(std::string text) {
  for (auto& character : text) {
    character = static_cast<char>(std::tolower(character));
  }

  return text;
}
}  // namespace
// vh: norm
LeftPane::LeftPane(std::shared_ptr<App> app)
    : app_{(cpp::Expects(app != nullptr), std::move(app))},
      open_file_dialog_{[]() {
        auto open_file_dialog =
            ImGui::FileBrowser{ImGuiFileBrowserFlags_CloseOnEsc};
        open_file_dialog.SetTitle("Open");
        open_file_dialog.SetTypeFilters({".json"});
        return open_file_dialog;
      }()},
      save_as_file_dialog_{[]() {
        auto save_as_file_dialog =
            ImGui::FileBrowser{ImGuiFileBrowserFlags_CloseOnEsc |
                               ImGuiFileBrowserFlags_EnterNewFilename |
                               ImGuiFileBrowserFlags_CreateNewDir};
        save_as_file_dialog.SetTitle("Save As");
        return save_as_file_dialog;
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

    DrawMenu(ImGui::GetContentRegionAvail().x);

    const auto selected_node_ids = esc::NodesAndLinks::GetSelectedNodeIds();

    {
      const auto indent_scope =
          cpp::Scope{[]() { ImGui::Indent(); }, []() { ImGui::Unindent(); }};

      for (auto& node : app_->GetNodesAndLinks().GetNodes()) {
        const auto is_node_selected =
            std::ranges::find(selected_node_ids, node->GetId()) !=
            selected_node_ids.end();

        DrawNode(*node, is_node_selected);
      }
    }
  }

  DrawDialog();
}
// vh: norm
void LeftPane::DrawMenu(float pane_width) {
  {
    const auto horizontal_scope = cpp::Scope{
        [pane_width]() {
          ImGui::BeginHorizontal("Style Editor", ImVec2{pane_width, 0});
        },
        []() { ImGui::EndHorizontal(); }};

    if (ImGui::Button("Open...")) {
      open_file_dialog_.Open();
    }

    if (ImGui::Button("Save As...")) {
      save_as_file_dialog_.Open();
    }

    if (ImGui::Button("Clear")) {
      app_->GetNodesAndLinks().DeleteAll();
    }
  }

  {
    const auto horizontal_scope = cpp::Scope{
        [pane_width]() {
          ImGui::BeginHorizontal("Style Editor", ImVec2{pane_width, 0});
        },
        []() { ImGui::EndHorizontal(); }};

    if (ImGui::Button("Zoom to Content")) {
      ne::NavigateToContent();
    }

    if (ImGui::Button("Show Flow")) {
      app_->ShowFlow();
    }
  }
}
// vh: norm
void LeftPane::DrawDialog() {
  open_file_dialog_.Display();

  if (open_file_dialog_.HasSelected()) {
    const auto selected_file_path = open_file_dialog_.GetSelected().string();
    open_file_dialog_.ClearSelected();

    app_->GetNodesAndLinks().LoadFromFile(selected_file_path);
  }

  save_as_file_dialog_.Display();

  if (save_as_file_dialog_.HasSelected()) {
    auto selected_file_path = save_as_file_dialog_.GetSelected().string();
    save_as_file_dialog_.ClearSelected();

    if (const auto not_json_extension =
            !AsLowerCase(selected_file_path).ends_with(".json")) {
      selected_file_path += ".json";
    }

    app_->GetNodesAndLinks().SafeToFile(selected_file_path);
  }
}
}  // namespace esc