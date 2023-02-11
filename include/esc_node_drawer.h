#ifndef VH_ESC_NODE_DRAWER_H_
#define VH_ESC_NODE_DRAWER_H_

#include <imgui_node_editor.h>

#include <optional>

#include "cpp_scope_function.h"
#include "esc_types.h"
#include "imgui.h"

namespace ne = ax::NodeEditor;

namespace esc {
class NodeDrawer {
 public:
  explicit NodeDrawer(ne::NodeId node_id);

  NodeDrawer(const NodeDrawer&) = delete;
  NodeDrawer(NodeDrawer&&) noexcept = default;

  auto operator=(const NodeDrawer&) noexcept -> NodeDrawer& = delete;
  auto operator=(NodeDrawer&&) noexcept -> NodeDrawer& = default;

  ~NodeDrawer();

  auto AddHeader(const TextureWithDims& header_texture,
                 const ImColor& header_color) -> cpp::ScopeFunction;

  auto AddPin(ne::PinId id, ne::PinKind kind) -> cpp::ScopeFunction;

 private:
  enum class Stage {
    kInvalid,
    kBegin,
    kHeader,
    kContent,
    kInput,
    kOutput,
    kEnd
  };

  struct Header {
    TextureWithDims texture{};
    ImColor color{};
    ImRect rect{};
  };

  void ProcessOldStage(Stage old_stage);
  void ProcessNewStage(Stage old_stage);
  void SetStage(Stage stage);

  void DrawHeader();

  ne::NodeId node_id_{};
  Stage stage_{};
  std::optional<Header> header_{};
};
}  // namespace esc

#endif  // VH_ESC_NODE_DRAWER_H_
