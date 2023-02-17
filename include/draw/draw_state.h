#ifndef VH_DRAW_STATE_H_
#define VH_DRAW_STATE_H_

// clang-format off
#include <imgui.h>
#include <imfilebrowser.h>
// clang-format on

#include <imgui_node_editor.h>

#include <optional>
#include <unordered_map>

#include "app_textures.h"
#include "core_state.h"
#include "draw_families_view.h"
#include "draw_flow_tree_view.h"
#include "draw_group_settings_view.h"
#include "draw_groups_view.h"
#include "draw_open_file_dialog.h"
#include "draw_save_as_file_dialog.h"
#include "draw_settings_view.h"
#include "flow_calculator.h"

namespace ne = ax::NodeEditor;

namespace esc::draw {
struct Rebind {
  ne::PinId fixed_pin{};
  ne::PinKind fixed_pin_kind{};
  ne::LinkId rebinding_link_id{};
  std::optional<ImVec2> fixed_pin_pos{};
};

struct NewLink {
  ne::PinId pin_dragged_from{};
  std::optional<ne::PinId> pin_hovered_over{};
  std::optional<Rebind> rebind{};
};

struct DrawState {
  FamiliesView families_view_{};
  FlowTreeView flow_tree_view_{};
  GroupsView groups_view_{};
  GroupSettingsView group_settings_view_{};
  SettingsView settings_view_{};
  
  OpenFileDialog open_file_dialog{};
  SaveAsFileDialog save_as_file_dialog{};









  std::optional<ne::NodeId> popup_node_{};

  bool color_flow{};
  float min{-27};
  float low{-22};
  float high{-18};
  float max{6};

  std::optional<NewLink> new_link{};

  std::unordered_map<uintptr_t, ImVec2> pin_poses_{};

  ImVec2 popup_position{};
  ne::NodeId popup_node_id{};
  ne::LinkId popup_link_id{};
  std::array<char, 100> popup_group_name{};

  Texture node_header_texture{};

  auto GetColorForFlowValue(float value) const -> ImColor;

  auto CanConnectFromPinToPin(core::CoreState &core_state, ne::PinId start_pin,
                              ne::PinId end_pin) -> bool;
};
}  // namespace esc::draw

#endif  // VH_DRAW_STATE_H_
