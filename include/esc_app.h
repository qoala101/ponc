#ifndef VH_ESC_APP_H_
#define VH_ESC_APP_H_

#include <application.h>
#include <imgui_node_editor.h>

#include <memory>
#include "esc_nodes_and_links.h"
#include "esc_textures_handle.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "esc_editor_context_handle.h"
#include "esc_types.h"

namespace ne = ax::NodeEditor;

class App : public Application {
 public:
  App(const char* name, int argc, char** argv);

  App(const App&) = delete;
  App(App&&) noexcept = delete;

  auto operator=(const App&) noexcept -> App& = delete;
  auto operator=(App&&) noexcept -> App& = delete;

  virtual ~App() = default;

 private:
  auto GetNextObjectId() -> int;
  auto GetNextLinkId() -> ne::LinkId;

  void TouchNode(ne::NodeId id);
  auto GetTouchProgress(ne::NodeId id) -> float;
  void UpdateTouch();


  void OnStart() override;
  void OnStop() override;
  void OnFrame(float deltaTime) override;

  void ShowLeftPane(float paneWidth);

  //

  auto CreateEditorConfig() -> ne::Config;

  void AddInitialNodes();
  void AddInitialLinks();

  std::optional<esc::EditorContextHandle> editor_context_{};
  std::optional<esc::TexturesHandle> textures_{};

  int next_object_id_{};

  esc::NodesAndLinks nodes_and_links_;

  std::map<ne::NodeId, float, NodeIdLess> node_touch_time_{};
  bool show_ordinals_{};
};

#endif  // VH_ESC_APP_H_
