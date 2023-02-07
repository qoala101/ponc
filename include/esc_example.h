#ifndef VH_ESC_EXAMPLE_H_
#define VH_ESC_EXAMPLE_H_

#include <application.h>
#include <imgui_node_editor.h>

#include <memory>

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

class Example : public Application {
 public:
  Example(const char* name, int argc, char** argv);

  Example(const Example&) = delete;
  Example(Example&&) noexcept = delete;

  auto operator=(const Example&) noexcept -> Example& = delete;
  auto operator=(Example&&) noexcept -> Example& = delete;

  virtual ~Example() = default;

 private:
  auto GetNextId() -> int;
  auto GetNextLinkId() -> ne::LinkId;

  void TouchNode(ne::NodeId id);
  auto GetTouchProgress(ne::NodeId id) -> float;
  void UpdateTouch();

  auto FindNode(ne::NodeId id) -> Node*;
  auto FindLink(ne::LinkId id) -> Link*;
  auto FindPin(ne::PinId id) -> Pin*;

  auto IsPinLinked(ne::PinId id) -> bool;

  void BuildNodes();

  auto SpawnInputActionNode() -> Node*;
  auto SpawnBranchNode() -> Node*;
  auto SpawnDoNNode() -> Node*;
  auto SpawnOutputActionNode() -> Node*;
  auto SpawnPrintStringNode() -> Node*;
  auto SpawnMessageNode() -> Node*;
  auto SpawnSetTimerNode() -> Node*;
  auto SpawnLessNode() -> Node*;
  auto SpawnWeirdNode() -> Node*;
  auto SpawnTraceByChannelNode() -> Node*;
  auto SpawnTreeSequenceNode() -> Node*;
  auto SpawnTreeTaskNode() -> Node*;
  auto SpawnTreeTask2Node() -> Node*;
  auto SpawnComment() -> Node*;
  auto SpawnHoudiniTransformNode() -> Node*;
  auto SpawnHoudiniGroupNode() -> Node*;

  void OnStart() override;
  void OnStop() override;
  void OnFrame(float deltaTime) override;

  void DrawPinIcon(const Pin& pin, bool connected, int alpha) const;

  void ShowLeftPane(float paneWidth);

  //

  auto CreateEditorConfig() -> ne::Config;

  void AddInitialNodes();
  void AddInitialLinks();

  void LoadTextures();
  void DestroyTextures();

  std::optional<esc::EditorContextHandle> editor_context_{};

  int next_id_{};

  std::vector<Node> nodes_{};
  std::vector<Link> links_{};

  struct TextureIds {
    ImTextureID header_background{};
    ImTextureID save_icon{};
    ImTextureID restore_icon{};
  } texture_ids_{};

  std::map<ne::NodeId, float, NodeIdLess> node_touch_time_{};
  bool show_ordinals_{};
};

#endif  // VH_ESC_EXAMPLE_H_
