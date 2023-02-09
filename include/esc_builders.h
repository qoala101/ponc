#ifndef VH_ESC_BUILDERS_H_
#define VH_ESC_BUILDERS_H_

#include <imgui_node_editor.h>

#include "esc_cpp.h"

namespace ne = ax::NodeEditor;

namespace esc {
struct BlueprintNodeBuilder {
  explicit BlueprintNodeBuilder(ne::NodeId node_id,
                                ImTextureID texture = nullptr,
                                int textureWidth = 0, int textureHeight = 0);

  BlueprintNodeBuilder(const BlueprintNodeBuilder&) = delete;
  BlueprintNodeBuilder(BlueprintNodeBuilder&&) noexcept = default;

  auto operator=(const BlueprintNodeBuilder&) noexcept
      -> BlueprintNodeBuilder& = delete;
  auto operator=(BlueprintNodeBuilder&&) noexcept
      -> BlueprintNodeBuilder& = default;

  ~BlueprintNodeBuilder();

  auto Header [[nodiscard]] (const ImVec4& color = ImVec4(1, 1, 1, 1))
  -> cpp::ScopeFunction;

  auto Input [[nodiscard]] (ne::PinId id) -> cpp::ScopeFunction;

  auto Output [[nodiscard]] (ne::PinId id) -> cpp::ScopeFunction;

 private:
  enum class Stage { Invalid, Begin, Header, Content, Input, Output, End };

  void EndHeader();
  void EndInput();
  void EndOutput();

  auto SetStage(Stage stage) -> bool;

  void Pin(ne::PinId id, ax::NodeEditor::PinKind kind);
  void EndPin();

  ImTextureID HeaderTextureId;
  int HeaderTextureWidth;
  int HeaderTextureHeight;
  ne::NodeId CurrentNodeId;
  Stage CurrentStage;
  ImU32 HeaderColor;
  ImVec2 NodeMin;
  ImVec2 NodeMax;
  ImVec2 HeaderMin;
  ImVec2 HeaderMax;
  ImVec2 ContentMin;
  ImVec2 ContentMax;
  bool HasHeader;
};
}  // namespace esc

#endif  // VH_ESC_BUILDERS_H_
