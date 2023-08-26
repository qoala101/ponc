/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_diagram_editor.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_node_editor.h>

#include <memory>
#include <vector>

#include "core_diagram.h"
#include "core_i_node.h"
#include "coreui_diagram.h"
#include "coreui_native_facade.h"
#include "cpp_assert.h"
#include "cpp_scope.h"
#include "draw_area.h"
#include "draw_area_creator.h"
#include "draw_background_popup.h"
#include "draw_linker.h"
#include "draw_links.h"
#include "draw_node.h"
#include "style_update_styles.h"

namespace vh::ponc::draw {
namespace {
///
auto IsHoveringOverChildWindow() {
  const auto *context = ImGui::GetCurrentContext();
  Expects(context != nullptr);
  return context->HoveredWindow != ImGui::GetCurrentWindow();
}

///
auto GetWindowFlags() {
  return ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
         ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings |
         ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus;
}
}  // namespace

///
DiagramEditor::DiagramEditor()
    : config_{[]() {
        auto config = ax::NodeEditor::Config{};
        config.SettingsFile = nullptr;
        return config;
      }()},
      context_{ne::CreateEditor(&config_), &ne::DestroyEditor} {
  ne::SetCurrentEditor(context_.get());
  style::UpdateStyle(ne::GetStyle());
}

///
void DiagramEditor::Draw(coreui::Diagram &diagram,
                         const core::Project &project) {
  // TODO(vh):
  // ImGui::SetNextWindowPos({}  // , ImGuiCond_FirstUseEver
  // );
  // ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize
  //                          // , ImGuiCond_FirstUseEver
  // );
  ImGui::Begin("Diagram Editor"
               // , nullptr, GetWindowFlags()
  );
  ne::Begin("Diagram Editor");
  item_deleter_.UnregisterDeletedItems(diagram.GetDiagram());

  auto &node_mover = diagram.GetNodeMover();

  for (auto &node : diagram.GetNodes()) {
    DrawNode(node, node_mover);
  }

  links_.Draw(diagram.GetLinks());

  auto &core_diagram = diagram.GetDiagram();

  for (auto &area : core_diagram.GetAreas()) {
    DrawArea(area, node_mover);
  }

  linker_.Draw(diagram.GetLinker(), diagram.GetFamilyGroups());

  OpenPopupsIfRequested(core_diagram);
  DrawPopups(diagram, project);

  item_deleter_.DeleteUnregisteredItems(diagram);
  ne::End();

  area_creator_.Draw(diagram.GetAreaCreator());
  ImGui::End();
}

///
auto DiagramEditor::OpenNodePopups(const core::Diagram &diagram) {
  auto requested_node_popup = false;
  auto node_id = ne::NodeId{};

  if (ne::ShowNodeContextMenu(&node_id)) {
    requested_node_popup = true;
  } else {
    auto pin_id = ne::PinId{};

    if (ne::ShowPinContextMenu(&pin_id)) {
      node_id = core::Diagram::FindPinNode(diagram, pin_id).GetId();
      requested_node_popup = true;
    }
  }

  if (requested_node_popup) {
    if (!ne::IsNodeSelected(node_id)) {
      coreui::NativeFacade::SelectNode(node_id);
    }

    node_popup_.Open();
    return true;
  }

  auto node_double_clicked = false;
  node_id = ne::GetDoubleClickedNode();

  if (node_id != ne::NodeId::Invalid) {
    if (!coreui::NativeFacade::IsArea(node_id)) {
      node_double_clicked = true;
    }
  } else {
    const auto double_clicked_pin = ne::GetDoubleClickedPin();

    if (double_clicked_pin != ne::PinId::Invalid) {
      node_id = core::Diagram::FindPinNode(diagram, double_clicked_pin).GetId();
      node_double_clicked = true;
    }
  }

  if (node_double_clicked) {
    replace_popup_.SetNodeId(node_id);
    replace_popup_.Open();
    return true;
  }

  return false;
}

///
void DiagramEditor::OpenLinkPopups() {
  auto link_id = ne::LinkId{};

  if (ne::ShowLinkContextMenu(&link_id)) {
    if (!ne::IsLinkSelected(link_id)) {
      ne::SelectLink(link_id);
    }

    link_popup_.Open();
    return;
  }

  link_id = ne::GetDoubleClickedLink();

  if (link_id == ne::LinkId::Invalid) {
    return;
  }

  edit_link_popup_.SetLinkId(link_id);
  edit_link_popup_.Open();
}

///
void DiagramEditor::OpenPopupsIfRequested(const core::Diagram &diagram) {
  const auto popup_pos = ImGui::GetMousePos();

  ne::Suspend();
  const auto resume_scope = cpp::Scope{[]() { ne::Resume(); }};

  if (ne::ShowBackgroundContextMenu() && !IsHoveringOverChildWindow()) {
    background_popup_.SetPos(popup_pos);
    background_popup_.Open();
    return;
  }

  if (OpenNodePopups(diagram)) {
    return;
  }

  OpenLinkPopups();
}

///
void DiagramEditor::DrawPopups(coreui::Diagram &diagram,
                               const core::Project &project) {
  background_popup_.Draw(diagram);
  node_popup_.Draw(diagram);
  link_popup_.Draw({.delete_selected = [&diagram](const auto &link_ids) {
    for (const auto link_id : link_ids) {
      diagram.DeleteLink(link_id);
    }
  }});
  replace_popup_.Draw(diagram);
  edit_link_popup_.Draw(diagram, project);
}
}  // namespace vh::ponc::draw