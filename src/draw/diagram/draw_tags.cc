/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_tags.h"

#include <imgui.h>

#include <iostream>

#include "draw_colored_text.h"
#include "style_default_colors.h"
#include "style_default_sizes.h"

namespace vh::ponc::draw {
void DrawTags(const coreui::Tags& tags) {
  for (auto rect : tags.GetRectangles()) {
    const auto padding = ImGui::GetStyle().FramePadding;
    rect.rect.Expand(padding);

    auto* drawList = ImGui::GetWindowDrawList();

    const auto color = style::Tailwind::GetColor(style::DefaultColors::kGray,
                                                 style::Tailwind::Shade::k200);

    drawList->AddRect(rect.rect.Min, rect.rect.Max, color,
                      ne::GetStyle().NodeRounding, 0,
                      style::DefaultSizes::kNormalThickness);

    rect.rect.Expand(padding);
    const auto text_size = ImGui::CalcTextSize(rect.text.c_str());
    rect.rect.Min.y -= text_size.y;

    drawList->AddRect(rect.rect.Min, rect.rect.Max, color,
                      ne::GetStyle().NodeRounding, 0,
                      style::DefaultSizes::kNormalThickness);

    auto text_pos = rect.rect.Min;
    text_pos.x += 24;

    ImGui::SetCursorPos(text_pos);
    ImGui::TextUnformatted(rect.text.c_str());
    // ImGui::SmallButton(rect.text.c_str());
  }
}
}  // namespace vh::ponc::draw