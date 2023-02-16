#include "esc_app.h"

#include <application.h>
#include <imgui_node_editor.h>
#include <sys/types.h>

#include <chrono>
#include <ios>
#include <memory>
#include <ranges>
#include <unordered_map>

#include "core_app.h"
#include "core_diagram.h"
#include "core_i_family.h"
#include "core_i_node.h"
#include "core_id_generator.h"
#include "cpp_assert.h"
#include "cpp_scope.h"
#include "draw_delete_items_process.h"
#include "draw_groups.h"
#include "draw_i_family_drawer.h"
#include "draw_i_node_drawer.h"
#include "draw_i_pin_drawer.h"
#include "draw_link_connection_process.h"
#include "draw_links.h"
#include "draw_main_window.h"
#include "draw_nodes.h"
#include "draw_popups.h"
#include "draw_repin_curve.h"
#include "draw_tooltip.h"
#include "esc_editor_context_handle.h"
#include "esc_node_drawer.h"
#include "esc_state.h"
#include "esc_textures_handle.h"
#include "imgui.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imfilebrowser.h>
#include <imgui_internal.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace esc {
// ---
App::App(const char* name, int argc, char** argv)
    : Application{name, argc, argv} {}

// ---
auto App::GetWindowFlags() const -> ImGuiWindowFlags {
  // NOLINTNEXTLINE(*-signed-bitwise)
  return Application::GetWindowFlags() | ImGuiWindowFlags_MenuBar;
}

// ---
void App::OnStart() {
  Expects(!impl_.has_value());
  impl_.emplace();
  Ensures(impl_.has_value());
}

// ---
void App::OnStop() {
  Expects(impl_.has_value());
  impl_.reset();
  Ensures(!impl_.has_value());
}

void App::OnFrame(float /*unused*/) {
  // assert(impl_.has_value());
  Expects(impl_.has_value());
  (*impl_).state_->OnFrame();
  (*impl_).main_window_.Draw(*(*impl_).state_);

  {
    const auto node_editor_scope =
        cpp::Scope{[]() { ne::Begin("Node editor"); }, []() { ne::End(); }};

    (*impl_).groups_.Draw(*(*impl_).state_);
    (*impl_).nodes_.Draw(*(*impl_).state_);
    (*impl_).links_.Draw(*(*impl_).state_);
    (*impl_).repin_curve_.Draw(*(*impl_).state_);
    (*impl_).link_connection_process_.Draw(*(*impl_).state_);
    (*impl_).delete_items_process_.Draw(*(*impl_).state_);
    (*impl_).popups_.Draw(*(*impl_).state_);
  }
  // ImGui::ShowDemoWindow();
  // ImGui::ShowMetricsWindow();
}
}  // namespace esc