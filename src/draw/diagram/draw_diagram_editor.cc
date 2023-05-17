#include "draw_diagram_editor.h"

#include <functional>
#include <iterator>
#include <memory>

#include "core_diagram.h"
#include "coreui_diagram.h"
#include "coreui_native_facade.h"
#include "cpp_assert.h"
#include "cpp_scope.h"
#include "draw_create_node_popup.h"
#include "draw_link.h"
#include "draw_linker.h"
#include "draw_node.h"
#include "imgui.h"
#include "imgui_node_editor.h"
#include "style_update_styles.h"

namespace vh::ponc::draw {
namespace {
///
auto IsHoveringOverChildWindow() {
  const auto *context = ImGui::GetCurrentContext();
  Expects(context != nullptr);
  return context->HoveredWindow != ImGui::GetCurrentWindow();
}
}  // namespace

///
DiagramEditor::DiagramEditor()
    : context_{ne::CreateEditor(), &ne::DestroyEditor} {
  ne::SetCurrentEditor(context_.get());
  style::UpdateStyle(ne::GetStyle());
}

///
void DiagramEditor::Draw(coreui::Diagram &diagram) {
  ne::Begin("DiagramEditor");
  item_deleter_.UnregisterDeletedItems(diagram.GetDiagram());

  auto &node_mover = diagram.GetNodeMover();

  for (auto &node : diagram.GetNodes()) {
    DrawNode(node, node_mover);
  }

  for (const auto &link : diagram.GetLinks()) {
    DrawLink(link);
  }

  linker_.Draw(diagram.GetLinker(), diagram.GetFamilyGroups());

  OpenPopupsIfRequested(diagram.GetDiagram());
  DrawPopups(diagram);

  item_deleter_.DeleteUnregisteredItems(diagram);
  ne::End();
}

///
void DiagramEditor::OpenPopupsIfRequested(const core::Diagram &diagram) {
  const auto popup_pos = ImGui::GetMousePos();

  ne::Suspend();
  const auto resume_scope = cpp::Scope{[]() { ne::Resume(); }};

  if (ne::ShowBackgroundContextMenu() && !IsHoveringOverChildWindow()) {
    create_node_popup_.SetPos(popup_pos);
    create_node_popup_.Open();
    return;
  }

  auto node_id = ne::NodeId{};
  auto requested_node_popup = false;

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
    return;
  }

  auto link_id = ne::LinkId{};

  if (ne::ShowLinkContextMenu(&link_id)) {
    if (!ne::IsLinkSelected(link_id)) {
      ne::SelectLink(link_id);
    }

    link_popup_.Open();
  }
}

///
void DiagramEditor::DrawPopups(coreui::Diagram &diagram) {
  create_node_popup_.Draw(diagram);
  node_popup_.Draw(diagram);
  link_popup_.Draw({.delete_selected = [&diagram](const auto &link_ids) {
    for (const auto link_id : link_ids) {
      diagram.DeleteLink(link_id);
    }
  }});
}
}  // namespace vh::ponc::draw