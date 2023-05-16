#include "draw_main_window.h"

#include "cpp_assert.h"
#include "draw_diagram_editor.h"
#include "draw_main_menu_bar.h"
#include "draw_recent_log.h"
#include "imgui.h"
#include "style_tailwind.h"
#include "style_utils.h"

namespace vh::ponc::draw {
void UpdateStyle() {
  using Color = style::Tailwind::Color;
  using Shade = style::Tailwind::Shade;

  constexpr const auto gray = Color::kZinc;
  constexpr const auto main = Color::kSky;

  struct Theme {
    ImGuiCol_ type{};
    Color color{};
    Shade shade{};
    float alpha{1};
  };

  constexpr const auto themes = std::array{
      Theme{ImGuiCol_Text, gray, Shade::k50},
      Theme{ImGuiCol_TextDisabled, gray, Shade::k500},
      Theme{ImGuiCol_WindowBg, gray, Shade::k900, 0.9},
      Theme{ImGuiCol_ChildBg, gray, Shade::k900, 0},
      Theme{ImGuiCol_PopupBg, gray, Shade::k800, 0.9},
      Theme{ImGuiCol_Border, gray, Shade::k500, 0.5},
      Theme{ImGuiCol_BorderShadow, gray, Shade::k900, 0},
      Theme{ImGuiCol_FrameBg, main, Shade::k900, 0.5},
      Theme{ImGuiCol_FrameBgHovered, main, Shade::k500, 0.4},
      Theme{ImGuiCol_FrameBgActive, main, Shade::k500, 0.7},
      Theme{ImGuiCol_TitleBg, gray, Shade::k900},
      Theme{ImGuiCol_TitleBgActive, main, Shade::k900},
      Theme{ImGuiCol_TitleBgCollapsed, gray, Shade::k900, 0.5},
      Theme{ImGuiCol_MenuBarBg, gray, Shade::k800},
      Theme{ImGuiCol_ScrollbarBg, gray, Shade::k900, 0.5},
      Theme{ImGuiCol_ScrollbarGrab, gray, Shade::k600},
      Theme{ImGuiCol_ScrollbarGrabHovered, gray, Shade::k500},
      Theme{ImGuiCol_ScrollbarGrabActive, gray, Shade::k400},
      Theme{ImGuiCol_CheckMark, main, Shade::k500},
      Theme{ImGuiCol_SliderGrab, main, Shade::k600},
      Theme{ImGuiCol_SliderGrabActive, main, Shade::k500},
      Theme{ImGuiCol_Button, main, Shade::k500, 0.4},
      Theme{ImGuiCol_ButtonHovered, main, Shade::k500},
      Theme{ImGuiCol_ButtonActive, main, Shade::k600},
      Theme{ImGuiCol_Header, main, Shade::k500, 0.3},
      Theme{ImGuiCol_HeaderHovered, main, Shade::k500, 0.8},
      Theme{ImGuiCol_HeaderActive, main, Shade::k500},
      Theme{ImGuiCol_Separator, gray, Shade::k500, 0.5},
      Theme{ImGuiCol_SeparatorHovered, main, Shade::k800, 0.8},
      Theme{ImGuiCol_SeparatorActive, main, Shade::k800},
      Theme{ImGuiCol_ResizeGrip, main, Shade::k500, 0.2},
      Theme{ImGuiCol_ResizeGripHovered, main, Shade::k500, 0.7},
      Theme{ImGuiCol_ResizeGripActive, main, Shade::k500, 0.9},
      Theme{ImGuiCol_Tab, main, Shade::k800},
      Theme{ImGuiCol_TabHovered, main, Shade::k500, 0.8},
      Theme{ImGuiCol_TabActive, main, Shade::k700},
      Theme{ImGuiCol_TabUnfocused, main, Shade::k900},
      Theme{ImGuiCol_TabUnfocusedActive, main, Shade::k800},
      Theme{ImGuiCol_PlotLines, gray, Shade::k400},
      Theme{ImGuiCol_PlotLinesHovered, Color::kOrange, Shade::k500},
      Theme{ImGuiCol_PlotHistogram, Color::kAmber, Shade::k400},
      Theme{ImGuiCol_PlotHistogramHovered, Color::kAmber, Shade::k500},
      Theme{ImGuiCol_TableHeaderBg, gray, Shade::k800},
      Theme{ImGuiCol_TableBorderStrong, gray, Shade::k600},
      Theme{ImGuiCol_TableBorderLight, gray, Shade::k700},
      Theme{ImGuiCol_TableRowBg, gray, Shade::k900, 0},
      Theme{ImGuiCol_TableRowBgAlt, gray, Shade::k50, 0.06},
      Theme{ImGuiCol_TextSelectedBg, main, Shade::k500, 0.35},
      Theme{ImGuiCol_DragDropTarget, Color::kYellow, Shade::k300, 0.9},
      Theme{ImGuiCol_NavHighlight, main, Shade::k500},
      Theme{ImGuiCol_NavWindowingHighlight, gray, Shade::k50, 0.7},
      Theme{ImGuiCol_NavWindowingDimBg, gray, Shade::k300, 0.2},
      Theme{ImGuiCol_ModalWindowDimBg, gray, Shade::k300, 0.35},
  };

  auto& colors = ImGui::GetStyle().Colors;
  Expects(std::size(colors) >= ImGuiCol_COUNT);

  for (const auto& theme : themes) {
    colors[theme.type] = style::WithAlpha(
        style::Tailwind::GetColor(theme.color, theme.shade), theme.alpha);
  }
}

///
void MainWindow::Draw(coreui::Project& project) {
  UpdateStyle();

  diagram_editor_.Draw(project.GetDiagram());
  main_menu_bar_.Draw(project);
  DrawRecentLog(project.GetLog(), main_menu_bar_.GetLogView());
}
}  // namespace vh::ponc::draw