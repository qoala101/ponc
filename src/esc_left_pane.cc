#include "esc_left_pane.h"

#include <bits/ranges_algo.h>
#include <imgui_node_editor.h>

#include <iostream>
#include <memory>
#include <utility>

#include "esc_app.h"
#include "esc_cpp.h"
#include "core_diagram.h"
#include "imgui.h"

namespace ne = ax::NodeEditor;

namespace esc {
namespace {
// vh: norm
void DrawNode(INode& node, bool is_selected) {
  const auto& io = ImGui::GetIO();

  {
    const auto id_scope =
        cpp::Scope{[&node]() { ImGui::PushID(node.GetId().AsPointer()); },
                   []() { ImGui::PopID(); }};

    const auto label = node.CreateDrawer()->GetLabel();

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
    : app_{(cpp::Expects(app != nullptr), std::move(app))} {
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

    const auto selected_node_ids = core::Diagram::GetSelectedNodeIds();

    {
      const auto indent_scope =
          cpp::Scope{[]() { ImGui::Indent(); }, []() { ImGui::Unindent(); }};

      for (auto& node : app_->GetDiagram().GetNodes()) {
        const auto is_node_selected =
            std::ranges::find(selected_node_ids, node->GetId()) !=
            selected_node_ids.end();

        DrawNode(*node, is_node_selected);
      }
    }
  }
}

// vh: norm
void LeftPane::DrawMenu(float pane_width) {
}
}  // namespace esc