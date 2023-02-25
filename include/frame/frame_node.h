#ifndef VH_FRAME_NODE_H_
#define VH_FRAME_NODE_H_

#include <imgui_node_editor.h>

#include <functional>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "core_id_generator.h"
#include "core_link.h"
#include "core_project.h"
#include "draw_texture.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "core_id_generator.h"

namespace ne = ax::NodeEditor;

namespace esc::frame {
struct Pin {
  std::optional<ne::PinId> id{};
  ImColor color{};
  bool filled{};
  std::string label{};
  std::variant<std::monostate, float, float *> value{};
};

struct NodeHeader {
  draw::Texture texture{};
  ImColor color{};
  std::string label{};
};

struct Node {
  ne::NodeId id{};
  std::optional<NodeHeader> header{};
  std::vector<Pin> input_pins{};
  std::vector<Pin> output_pins{};
};

struct Link {
  ne::LinkId id{};
  ne::PinId start_pin_id{};
  ne::PinId end_pin_id{};
  ImColor color{};
  float thickness{};
};

struct Curve {
  std::optional<ImVec2> start_position{};
  std::optional<ImVec2> end_position{};
  ImColor color{};
  float thickness{};
};

struct Creation {
  bool can_connect{};
  std::string reason{};

  std::optional<ne::LinkId> delete_link{};
  ne::PinId start_pin_id{};
  ne::PinId end_pin_id{};
};

struct NewLink {
  std::optional<ne::PinId> dragged_from_pin_{};
  std::optional<ne::PinId> hovering_over_pin_{};
};

struct Frame {
  Frame(core::IdGenerator &id_generator, core::Project &project);

  ~Frame();

  NewLink new_link{};
  std::vector<Node> nodes{};
  std::vector<Link> links{};
  std::optional<Curve> curve{};
  float creation_alpha{};
  std::optional<Creation> creation{};
  std::unordered_map<uintptr_t, ImRect> drawn_pin_icon_rects_{};

  auto GetProject() -> core::Project &;
  auto GetIdGenerator() -> core::IdGenerator &;

  void OpenProjectFromFile(std::string file_path);
  void SaveProjectToFile(std::string file_path);
  void EmplaceNode(std::shared_ptr<core::INode> node, const ImVec2 &position);
  void CreateLink(ne::PinId start_pin_id, ne::PinId end_pin_id);
  void DeleteLink(ne::LinkId link_id);

 private:
  core::IdGenerator &id_generator_;
  core::Project &project_;
  std::vector<std::function<void()>> events_{};
};
}  // namespace esc::frame

#endif  // VH_FRAME_NODE_H_
