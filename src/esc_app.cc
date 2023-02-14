#include "esc_app.h"

#include <application.h>
#include <bits/ranges_algo.h>
#include <imgui_node_editor.h>
#include <sys/types.h>

#include <chrono>
#include <ios>
#include <memory>
#include <unordered_map>

#include "core_diagram.h"
#include "core_i_family.h"
#include "core_i_node.h"
#include "core_id_generator.h"
#include "cpp_assert.h"
#include "cpp_scope.h"
#include "draw_i_family_drawer.h"
#include "draw_i_node_drawer.h"
#include "draw_i_pin_drawer.h"
#include "draw_tooltip.h"
#include "esc_node_drawer.h"
#include "esc_state.h"
#include "esc_textures_handle.h"
#include "esc_types.h"
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

namespace ne = ax::NodeEditor;

namespace esc {
namespace {}  // namespace
// vh: norm
App::App(const char* name, int argc, char** argv)
    : Application{name, argc, argv} {}
// vh: norm
void App::OnStart() {
  state_.emplace(std::make_shared<State>());
  editor_context_.emplace();
  nodes_.emplace(std::move(TexturesHandle{shared_from_this()}));
  main_window_.emplace();
  popups_.emplace();
  links_.emplace();
  link_connection_process_.emplace();
}
// vh: norm
void App::OnStop() {
  link_connection_process_.reset();
  links_.reset();
  popups_.reset();
  main_window_.reset();
  nodes_.reset();
  editor_context_.reset();
  state_.reset();
}

auto App::GetWindowFlags() const -> ImGuiWindowFlags {
  return Application::GetWindowFlags() | ImGuiWindowFlags_MenuBar;
}

// vh: ok
void App::OnFrame(float /*unused*/) {
  (*state_)->OnFrame();
  DrawFrame();
}
// vh: norm
void App::DrawDeleteItemsProcess() {
  const auto delete_scope = cpp::Scope{[]() { ne::EndDelete(); }};

  if (ne::BeginDelete()) {
    auto link_id = ne::LinkId{};

    while (ne::QueryDeletedLink(&link_id)) {
      if (ne::AcceptDeletedItem()) {
      }
    }

    auto node_id = ne::NodeId{};

    while (ne::QueryDeletedNode(&node_id)) {
      if (ne::AcceptDeletedItem()) {
      }
    }
  }
}
// vh: norm
void App::DrawNodeEditor() {
  const auto node_editor_scope =
      cpp::Scope{[]() { ne::Begin("Node editor"); }, []() { ne::End(); }};

  nodes_->Draw(**state_);
  links_->Draw(**state_);
  link_connection_process_->Draw(**state_);
  DrawDeleteItemsProcess();
  popups_->Draw(**state_);
}
// vh: bad
void App::DrawFrame() {
  main_window_->Draw(**state_);
  DrawNodeEditor();

  ImGui::ShowDemoWindow();
  ImGui::ShowMetricsWindow();
}
}  // namespace esc