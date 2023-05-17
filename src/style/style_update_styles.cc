#include "style_update_styles.h"

#include "style_default_colors.h"
#include "style_tailwind.h"

namespace vh::ponc::style {
///
using Color = style::Tailwind::Color;
///
using Shade = style::Tailwind::Shade;

///
void UpdateStyle(ImGuiStyle& style) {
  struct Item {
    ImGuiCol_ item{};
    Color color{};
    Shade shade{};
    float alpha{1};
  };

  constexpr auto items = std::array{
      Item{ImGuiCol_Text, DefaultColors::kGray, Shade::k50},
      Item{ImGuiCol_TextDisabled, DefaultColors::kGray, Shade::k500},
      Item{ImGuiCol_WindowBg, DefaultColors::kGray, Shade::k900, 0.9},
      Item{ImGuiCol_ChildBg, DefaultColors::kGray, Shade::k900, 0},
      Item{ImGuiCol_PopupBg, DefaultColors::kGray, Shade::k800, 0.9},
      Item{ImGuiCol_Border, DefaultColors::kGray, Shade::k500, 0.5},
      Item{ImGuiCol_BorderShadow, DefaultColors::kGray, Shade::k900, 0},
      Item{ImGuiCol_FrameBg, DefaultColors::kGray, Shade::k600, 0.5},
      Item{ImGuiCol_FrameBgHovered, DefaultColors::kAccent, Shade::k500, 0.4},
      Item{ImGuiCol_FrameBgActive, DefaultColors::kAccent, Shade::k500, 0.7},
      Item{ImGuiCol_TitleBg, DefaultColors::kGray, Shade::k900},
      Item{ImGuiCol_TitleBgActive, DefaultColors::kGray, Shade::k700},
      Item{ImGuiCol_TitleBgCollapsed, DefaultColors::kGray, Shade::k900, 0.5},
      Item{ImGuiCol_MenuBarBg, DefaultColors::kGray, Shade::k800},
      Item{ImGuiCol_ScrollbarBg, DefaultColors::kGray, Shade::k900, 0.5},
      Item{ImGuiCol_ScrollbarGrab, DefaultColors::kGray, Shade::k600},
      Item{ImGuiCol_ScrollbarGrabHovered, DefaultColors::kGray, Shade::k500},
      Item{ImGuiCol_ScrollbarGrabActive, DefaultColors::kGray, Shade::k400},
      Item{ImGuiCol_CheckMark, DefaultColors::kAccent, Shade::k600},
      Item{ImGuiCol_SliderGrab, DefaultColors::kAccent, Shade::k600},
      Item{ImGuiCol_SliderGrabActive, DefaultColors::kAccent, Shade::k500},
      Item{ImGuiCol_Button, DefaultColors::kAccent, Shade::k500, 0.4},
      Item{ImGuiCol_ButtonHovered, DefaultColors::kAccent, Shade::k600},
      Item{ImGuiCol_ButtonActive, DefaultColors::kAccent, Shade::k500},
      Item{ImGuiCol_Header, DefaultColors::kAccent, Shade::k400, 0.3},
      Item{ImGuiCol_HeaderHovered, DefaultColors::kAccent, Shade::k600, 0.8},
      Item{ImGuiCol_HeaderActive, DefaultColors::kAccent, Shade::k600},
      Item{ImGuiCol_Separator, DefaultColors::kGray, Shade::k500, 0.5},
      Item{ImGuiCol_SeparatorHovered, DefaultColors::kAccent, Shade::k800, 0.8},
      Item{ImGuiCol_SeparatorActive, DefaultColors::kAccent, Shade::k800},
      Item{ImGuiCol_ResizeGrip, DefaultColors::kGray, Shade::k500, 0.2},
      Item{ImGuiCol_ResizeGripHovered, DefaultColors::kGray, Shade::k500, 0.7},
      Item{ImGuiCol_ResizeGripActive, DefaultColors::kGray, Shade::k500, 0.9},
      Item{ImGuiCol_Tab, DefaultColors::kAccent, Shade::k800, 0.86},
      Item{ImGuiCol_TabHovered, DefaultColors::kAccent, Shade::k500, 0.8},
      Item{ImGuiCol_TabActive, DefaultColors::kAccent, Shade::k700},
      Item{ImGuiCol_TabUnfocused, DefaultColors::kAccent, Shade::k950},
      Item{ImGuiCol_TabUnfocusedActive, DefaultColors::kAccent, Shade::k900},
      Item{ImGuiCol_PlotLines, DefaultColors::kGray, Shade::k400},
      Item{ImGuiCol_PlotLinesHovered, Color::kOrange, Shade::k500},
      Item{ImGuiCol_PlotHistogram, Color::kAmber, Shade::k400},
      Item{ImGuiCol_PlotHistogramHovered, Color::kAmber, Shade::k500},
      Item{ImGuiCol_TableHeaderBg, DefaultColors::kGray, Shade::k800},
      Item{ImGuiCol_TableBorderStrong, DefaultColors::kGray, Shade::k600},
      Item{ImGuiCol_TableBorderLight, DefaultColors::kGray, Shade::k700},
      Item{ImGuiCol_TableRowBg, DefaultColors::kGray, Shade::k900, 0},
      Item{ImGuiCol_TableRowBgAlt, DefaultColors::kGray, Shade::k50, 0.06},
      Item{ImGuiCol_TextSelectedBg, DefaultColors::kAccent, Shade::k500, 0.35},
      Item{ImGuiCol_DragDropTarget, Color::kYellow, Shade::k300, 0.9},
      Item{ImGuiCol_NavHighlight, DefaultColors::kAccent, Shade::k500},
      Item{ImGuiCol_NavWindowingHighlight, DefaultColors::kGray, Shade::k50,
           0.7},
      Item{ImGuiCol_NavWindowingDimBg, DefaultColors::kGray, Shade::k300, 0.2},
      Item{ImGuiCol_ModalWindowDimBg, DefaultColors::kGray, Shade::k300, 0.35},
  };

  Expects(std::size(style.Colors) >= ImGuiCol_COUNT);

  for (const auto& item : items) {
    // NOLINTNEXTLINE(*-constant-array-index)
    style.Colors[item.item] = style::WithAlpha(
        style::Tailwind::GetColor(item.color, item.shade), item.alpha);
  }
}

///
void UpdateStyle(ne::Style& style) {
  style.NodeRounding = 8;
  style.NodeBorderWidth = 1;
  style.HoveredNodeBorderWidth = 4;
  style.SelectedNodeBorderWidth = 4;
  style.ScrollDuration = 0.25;

  struct Item {
    ne::StyleColor item{};
    Color color{};
    Shade shade{};
    float alpha{1};
  };

  constexpr auto items = std::array{
      Item{ne::StyleColor_Bg, DefaultColors::kGray, Shade::k600},
      Item{ne::StyleColor_Grid, DefaultColors::kGray, Shade::k700},
      Item{ne::StyleColor_NodeBg, DefaultColors::kGray, Shade::k900, 0.75},
      Item{ne::StyleColor_NodeBorder, DefaultColors::kGray, Shade::k500, 0.75},
      Item{ne::StyleColor_HovNodeBorder, Color::kCyan, Shade::k500},
      Item{ne::StyleColor_SelNodeBorder, Color::kAmber, Shade::k500},
      Item{ne::StyleColor_NodeSelRect, Color::kBlue, Shade::k500, 0.25},
      Item{ne::StyleColor_NodeSelRectBorder, Color::kBlue, Shade::k500},
      Item{ne::StyleColor_HovLinkBorder, Color::kCyan, Shade::k500},
      Item{ne::StyleColor_SelLinkBorder, Color::kAmber, Shade::k500},
      Item{ne::StyleColor_PinRect, Color::kCyan, Shade::k500, 0.5},
  };

  Expects(std::size(style.Colors) >= ne::StyleColor_Count);

  for (const auto& theme : items) {
    // NOLINTNEXTLINE(*-constant-array-index)
    style.Colors[theme.item] = style::WithAlpha(
        style::Tailwind::GetColor(theme.color, theme.shade), theme.alpha);
  }
}
}  // namespace vh::ponc::style