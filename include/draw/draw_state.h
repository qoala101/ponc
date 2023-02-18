#ifndef VH_DRAW_STATE_H_
#define VH_DRAW_STATE_H_

#include <imgui_node_editor.h>

#include <optional>
#include <unordered_map>

#include "app_textures.h"
#include "core_state.h"
#include "draw_background_popup.h"
#include "draw_families_view.h"
#include "draw_flow_tree_view.h"
#include "draw_group_settings_view.h"
#include "draw_groups_view.h"
#include "draw_link_being_repinned.h"
#include "draw_link_connection_process.h"
#include "draw_link_popup.h"
#include "draw_links.h"
#include "draw_node_popup.h"
#include "draw_nodes.h"
#include "draw_open_file_dialog.h"
#include "draw_save_as_file_dialog.h"
#include "draw_settings_view.h"
#include "flow_calculator.h"

namespace ne = ax::NodeEditor;

namespace esc::draw {
struct DrawState {
  explicit DrawState(const Texture &node_header_texture);

  FamiliesView families_view_{};
  FlowTreeView flow_tree_view_{};
  GroupsView groups_view_{};
  GroupSettingsView group_settings_view_{};
  SettingsView settings_view_{};

  OpenFileDialog open_file_dialog{};
  SaveAsFileDialog save_as_file_dialog{};

  BackgroundPopup background_popup{};
  NodePopup node_popup{};
  LinkPopup link_popup{};

  Nodes nodes;
  Links links;
  LinkBeingRepinned link_being_repinned;
  LinkConnectionProcess link_connection_process;

  std::unordered_map<uintptr_t, ImVec2> pin_poses_{};

 private:
  DrawState(const Texture &node_header_texture,
            std::shared_ptr<std::optional<NewLink>> new_link);
};
}  // namespace esc::draw

#endif  // VH_DRAW_STATE_H_
