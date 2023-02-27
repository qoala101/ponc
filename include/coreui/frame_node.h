#ifndef VH_FRAME_NODE_H_
#define VH_FRAME_NODE_H_

#include <imgui_node_editor.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "core_i_family.h"
#include "core_i_node.h"
#include "core_id_generator.h"
#include "core_link.h"
#include "core_project.h"
#include "coreui_i_pin_traits.h"
#include "coreui_texture.h"
#include "imgui.h"
#include "imgui_internal.h"

namespace ne = ax::NodeEditor;

namespace esc::coreui {
struct Pin {
  std::optional<ne::PinId> id{};
  std::variant<std::monostate, float, float*> value{};
  std::string label{};
  ImColor color{};
  bool filled{};
};

struct NodeHeader {
  std::string label{};
  ImColor color{};
  Texture texture{};
};

struct Node {
  ne::NodeId id{};
  std::optional<NodeHeader> header{};
  std::vector<Pin> input_pins{};
  std::vector<Pin> output_pins{};
};

struct Link {
  ne::LinkId id{};
  ne::PinId start_pin{};
  ne::PinId end_pin{};
  ImColor color{};
  float thickness{};
};

struct ArtificialLink {
  std::optional<ImVec2> start_pos{};
  std::optional<ImVec2> end_pos{};
  ImColor color{};
  float thickness{};
};

class Frame;

struct NewLink {
  ne::PinId dragged_from_pin_{};
  std::optional<ne::PinId> hovering_over_pin_{};
};

struct Creation {
  explicit Creation(Frame* frame, const std::optional<NewLink>& new_link);

  float alpha{1.F};
  bool can_connect{};
  std::string reason{};

 private:
  friend class Frame;

  std::optional<ne::PinId> fixed_pin{};
  std::optional<ne::LinkId> delete_link{};
  ne::PinId start_pin_id{};
  ne::PinId end_pin_id{};
};

struct Nodes {
  std::vector<Node> nodes{};
  std::unordered_map<uintptr_t, ImRect> pin_rects{};
};

struct Links {
  std::vector<Link> links{};
  std::optional<ArtificialLink> artificial_link{};
};

class Frame {
  friend struct Creation;

 public:
  explicit Frame(core::Project* project);

  auto GetProject() -> core::Project&;

 public:
  core::Project* project_;
  std::vector<std::function<void()>> events_{};

  std::optional<NewLink> new_link{};
  Creation creation;
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
  void UpdateCreation();
  void UpdateNodes();
  void UpdateLinks();

 private:
  auto GetLinkAlpha(const core::Link& link);

  auto GetColorForFlowValue(float value) -> ImColor;

  auto FindFixedPin() -> std::optional<ne::PinId>;

  auto GetLinks();

  auto GetPinKind(const coreui::IPinTraits& pin_drawer,
                  const core::INode& node);

  auto GetCanConnectToPinReason(ne::PinId pin_id)
      -> std::pair<bool, std::string>;

  auto CanConnectToPin(ne::PinId pin_id) -> bool;

  auto GetPinIconAlpha(ne::PinId pin_id);

  auto GetNodes();

  auto GetCurve() -> std::optional<ArtificialLink>;

  auto GetAlphaForNewLink() -> float;
};
}  // namespace esc::coreui

#endif  // VH_FRAME_NODE_H_
