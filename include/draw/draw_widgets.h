#ifndef VH_DRAW_WIDGETS_H_
#define VH_DRAW_WIDGETS_H_

#include <imgui_node_editor.h>

#include <optional>
#include <unordered_map>

#include "app_event_queue.h"
#include "core_id_generator.h"
#include "core_project.h"
#include "draw_node_editor.h"
#include "draw_texture.h"
#include "flow_calculator.h"

namespace esc {
class EventQueue;

namespace core {
class Project;
class IdGenerator;
}  // namespace core

namespace flow {
class FlowCalculator;
}  // namespace flow

namespace draw {
// ---
struct Widgets {
  // ---
  explicit Widgets(const Texture &node_header_texture);

  // ---
  void Draw(core::Project &project, core::IdGenerator &id_generator,
            flow::FlowCalculator &flow_calculator, EventQueue &event_queue);

  // // ---
  // OpenFileDialog open_file_dialog{};
  // // ---
  // SaveAsFileDialog save_as_file_dialog{};

  // // ---
  // FamiliesView families_view{};
  // // ---
  // FlowTreeView flow_tree_view{};
  // // ---
  // GroupsView groups_view{};
  // // ---
  // GroupSettingsView group_settings_view{};
  // // ---
  // SettingsView settings_view{};

  // ---
  NodeEditor node_editor{};
  // // ---
  // Nodes nodes;
  // // ---
  // Links links;
  // // ---
  // LinkBeingRepinned link_being_repinned;
  // // ---
  // LinkConnectionProcess link_connection_process;

  // // ---
  // BackgroundPopup background_popup{};
  // // ---
  // NodePopup node_popup{};
  // // ---
  // LinkPopup link_popup{};
};
}  // namespace draw
}  // namespace esc

#endif  // VH_DRAW_WIDGETS_H_
