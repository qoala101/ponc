#ifndef VH_FRAME_NODE_H_
#define VH_FRAME_NODE_H_

#include <imgui_node_editor.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "core_diagram.h"
#include "core_i_family.h"
#include "core_i_node.h"
#include "core_id_generator.h"
#include "core_link.h"
#include "core_project.h"
#include "coreui_handmade_link.h"
#include "coreui_i_pin_traits.h"
#include "coreui_link_creation.h"
#include "coreui_texture.h"
#include "imgui.h"
#include "imgui_internal.h"

namespace ne = ax::NodeEditor;

namespace esc::coreui {
struct NodeHeader {
  std::string label{};
  ImColor color{};
  Texture texture{};
};

struct Pin {
  std::optional<ne::PinId> id{};
  std::variant<std::monostate, float, float*> value{};
  std::string label{};
  ImColor color{};
  bool filled{};
};

struct Node {
  ne::NodeId id{};
  std::optional<NodeHeader> header{};
  std::vector<Pin> input_pins{};
  std::vector<Pin> output_pins{};
};

struct Nodes {
  std::vector<Node> nodes{};
  std::unordered_map<uintptr_t, ImRect> pin_rects{};
};

struct Link {
  ne::LinkId id{};
  ne::PinId start_pin{};
  ne::PinId end_pin{};
  ImColor color{};
  float thickness{};
};

struct HandmadeLink {
  std::optional<ImVec2> start_pos{};
  std::optional<ImVec2> end_pos{};
  ImColor color{};
  float thickness{};
};

struct Links {
  std::vector<Link> links{};
  std::optional<HandmadeLink> handmade_link{};
};

class Frame {
 public:
  explicit Frame(core::Project* project);

  auto GetProject() const -> core::Project&;

 public:
  core::Project* project_;
  std::vector<std::function<void()>> events_{};

  LinkCreation creation;
  Nodes nodes{};
  Links links{};

 public:
  void OnFrame();

  void OpenProjectFromFile(std::string file_path);
  void SaveProjectToFile(std::string file_path);
  void EmplaceNode(std::shared_ptr<core::INode> node, const ImVec2& position);
  void CreateLink(ne::PinId start_pin_id, ne::PinId end_pin_id);
  void DeleteLink(ne::LinkId link_id);

  void CreateCurrentLink();

 private:
  void UpdateNodes();
  void UpdateLinks();

  auto GetLinkAlpha(ne::LinkId link_id) const;
  auto GetColorForFlowValue(float value) const;

  auto GetLinks();

  auto GetPinIconAlpha(ne::PinId pin_id) const;

  auto GetNodes();
  auto GetCurve() -> std::optional<HandmadeLink>;

 private:
  void SlotPinsUpdated();
};
}  // namespace esc::coreui

#endif  // VH_FRAME_NODE_H_
