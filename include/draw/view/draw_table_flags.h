#ifndef VH_PONC_DRAW_TABLE_FLAGS_H_
#define VH_PONC_DRAW_TABLE_FLAGS_H_

#include <imgui.h>

namespace vh::ponc::draw {
// NOLINTBEGIN(*-signed-bitwise)
///
static constexpr auto kFixedTableFlags =
    ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
    ImGuiTableFlags_Hideable | ImGuiTableFlags_ContextMenuInBody |
    ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersOuter;
///
static constexpr auto kExpandingTableFlags =
    kFixedTableFlags | ImGuiTableFlags_ScrollY;
// NOLINTEND(*-signed-bitwise)
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_TABLE_FLAGS_H_
