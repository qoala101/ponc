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
#include "style_tailwind.h"
#include "style_utils.h"

namespace vh::ponc::draw {
namespace {
///
auto IsHoveringOverChildWindow() {
  const auto *context = ImGui::GetCurrentContext();
  Expects(context != nullptr);
  return context->HoveredWindow != ImGui::GetCurrentWindow();
}

void UpdateStyle() {
  auto &style = ne::GetStyle();

  style.NodeRounding = 8;
  style.NodeBorderWidth = 1;
  style.HoveredNodeBorderWidth = 4;
  style.SelectedNodeBorderWidth = 4;
  style.ScrollDuration = 0.25;

  using Color = style::Tailwind::Color;
  using Shade = style::Tailwind::Shade;

  constexpr const auto gray = Color::kZinc;

  struct Theme {
    ne::StyleColor type{};
    Color color{};
    Shade shade{};
    float alpha{1};
  };

  constexpr const auto themes = std::array{
      Theme{ne::StyleColor_Bg, gray, Shade::k600},
      Theme{ne::StyleColor_Grid, gray, Shade::k700},
      Theme{ne::StyleColor_NodeBg, gray, Shade::k900, 0.75},
      Theme{ne::StyleColor_NodeBorder, gray, Shade::k500, 0.75},
      Theme{ne::StyleColor_HovNodeBorder, Color::kCyan, Shade::k500},
      Theme{ne::StyleColor_SelNodeBorder, Color::kAmber, Shade::k500},
      Theme{ne::StyleColor_NodeSelRect, Color::kBlue, Shade::k500, 0.25},
      Theme{ne::StyleColor_NodeSelRectBorder, Color::kBlue, Shade::k500},
      Theme{ne::StyleColor_HovLinkBorder, Color::kCyan, Shade::k500},
      Theme{ne::StyleColor_SelLinkBorder, Color::kAmber, Shade::k500},
      Theme{ne::StyleColor_PinRect, Color::kCyan, Shade::k500, 0.5},
  };

  auto &colors = style.Colors;
  Expects(std::size(colors) >= ne::StyleColor_Count);

  for (const auto &theme : themes) {
    colors[theme.type] = style::WithAlpha(
        style::Tailwind::GetColor(theme.color, theme.shade), theme.alpha);
  }
}
}  // namespace

///
DiagramEditor::DiagramEditor()
    : context_{ne::CreateEditor(), &ne::DestroyEditor} {
  ne::SetCurrentEditor(context_.get());
  UpdateStyle();
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