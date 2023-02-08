#include "esc_nodes_and_links.h"

#include <crude_json.h>
#include <imgui_node_editor.h>
#include <imgui_node_editor_internal.h>

#include <iostream>
#include <string>

#include "esc_app.h"
#include "esc_cpp.h"

namespace esc {
namespace {
void BuildNode(Node* node) {
  for (auto& input : node->Inputs) {
    input.Node = node;
    input.Kind = PinKind::Input;
  }

  for (auto& output : node->Outputs) {
    output.Node = node;
    output.Kind = PinKind::Output;
  }
}
}  // namespace

NodesAndLinks::NodesAndLinks(std::shared_ptr<App> app)
    : app_{(cpp::Expects(app != nullptr), std::move(app))} {
  cpp::Ensures(app_ != nullptr);
}

auto NodesAndLinks::SpawnInputNode() -> Node* {
  auto& node = nodes_.emplace_back(app_->GetNextObjectId(), "Input",
                                   ImColor{255, 127, 127});

  node.Outputs.emplace_back(app_->GetNextObjectId(), "6 dB", PinType::Flow);

  BuildNode(&node);
  return &node;
}

auto NodesAndLinks::Spawn1ToNNode(int n) -> Node* {
  const auto n_string = std::to_string(n);
  const auto node_name = "1/" + n_string;

  auto& node = nodes_.emplace_back(app_->GetNextObjectId(), node_name.c_str(),
                                   ImColor{127 / n, 127 / n, 255});

  node.Inputs.emplace_back(app_->GetNextObjectId(), "In", PinType::Flow);

  for (auto i = 0; i < n; ++i) {
    const auto out_name = "Out " + n_string;
    node.Outputs.emplace_back(app_->GetNextObjectId(), out_name.c_str(),
                              PinType::Flow);
  }

  BuildNode(&node);
  return &node;
}

auto NodesAndLinks::Spawn1To2Node() -> Node* { return Spawn1ToNNode(2); }

auto NodesAndLinks::Spawn1To4Node() -> Node* { return Spawn1ToNNode(4); }

auto NodesAndLinks::Spawn1To8Node() -> Node* { return Spawn1ToNNode(8); }

auto NodesAndLinks::Spawn1To16Node() -> Node* { return Spawn1ToNNode(16); }

auto NodesAndLinks::SpawnClientNode() -> Node* {
  auto& node = nodes_.emplace_back(app_->GetNextObjectId(), "Client",
                                   ImColor{127, 255, 127});

  node.Inputs.emplace_back(app_->GetNextObjectId(), "In", PinType::Flow);

  BuildNode(&node);
  return &node;
}

auto NodesAndLinks::SpawnCommentNode() -> Node* {
  auto& node = nodes_.emplace_back(app_->GetNextObjectId(), "Comment");

  node.Type = NodeType::Comment;
  node.Size = ImVec2{300, 200};

  return &node;
}

void NodesAndLinks::BuildNodes() {
  for (auto& node : nodes_) {
    BuildNode(&node);
  }
}

auto NodesAndLinks::GetNodes() -> std::vector<Node>& { return nodes_; }

auto NodesAndLinks::GetLinks() const -> const std::vector<Link>& {
  return links_;
}

auto NodesAndLinks::FindNode(ne::NodeId id) -> Node* {
  for (auto& node : nodes_)
    if (node.ID == id) return &node;

  return nullptr;
}

auto NodesAndLinks::FindPin(ne::PinId id) -> Pin* {
  if (!id) return nullptr;

  for (auto& node : nodes_) {
    for (auto& pin : node.Inputs)
      if (pin.ID == id) return &pin;

    for (auto& pin : node.Outputs)
      if (pin.ID == id) return &pin;
  }

  return nullptr;
}

auto NodesAndLinks::FindLink(ne::LinkId id) -> Link* {
  for (auto& link : links_)
    if (link.ID == id) return &link;

  return nullptr;
}

auto NodesAndLinks::IsPinLinked(ne::PinId id) -> bool {
  if (!id) return false;

  for (auto& link : links_)
    if (link.StartPinID == id || link.EndPinID == id) return true;

  return false;
}

void NodesAndLinks::SpawnLink(const Link& link) { links_.emplace_back(link); }

void NodesAndLinks::EraseLinkWithId(ne::LinkId linkId) {
  auto id = std::find_if(links_.begin(), links_.end(),
                         [linkId](auto& link) { return link.ID == linkId; });
  if (id != links_.end()) links_.erase(id);
}

auto NodesAndLinks::GetSelectedNodeIds() -> std::vector<ne::NodeId> {
  const auto num_selected_objects = ne::GetSelectedObjectCount();

  auto selected_ids = std::vector<ne::NodeId>{};
  selected_ids.resize(num_selected_objects);

  const auto num_selected_nodes = ne::GetSelectedNodes(
      selected_ids.data(), static_cast<int>(selected_ids.size()));
  selected_ids.resize(num_selected_nodes);

  return selected_ids;
}

auto NodesAndLinks::GetSelectedLinkIds() -> std::vector<ne::LinkId> {
  const auto num_selected_objects = ne::GetSelectedObjectCount();

  auto selected_ids = std::vector<ne::LinkId>{};
  selected_ids.resize(num_selected_objects);

  const auto num_selected_links = ne::GetSelectedLinks(
      selected_ids.data(), static_cast<int>(selected_ids.size()));
  selected_ids.resize(num_selected_links);

  return selected_ids;
}

auto NodesAndLinks::SpawnNodeByTypeName(const std::string& type_name) -> Node* {
  if (type_name == "Input") {
    return SpawnInputNode();
  }

  if (type_name == "1/2") {
    return Spawn1To2Node();
  }

  if (type_name == "1/4") {
    return Spawn1To4Node();
  }

  if (type_name == "1/8") {
    return Spawn1To8Node();
  }

  if (type_name == "1/16") {
    return Spawn1To16Node();
  }

  if (type_name == "Client") {
    return SpawnClientNode();
  }

  if (type_name == "Comment") {
    return SpawnCommentNode();
  }
}

auto NodesAndLinks::GetNodeTypeNames() -> std::vector<std::string> {
  return {"Input", "1/2", "1/4", "1/8", "1/16", "Client", "Comment"};
}

void NodesAndLinks::SpawnLinkFromPinToNode(const Pin* pin, const Node* node) {
  const auto& node_pins =
      (pin->Kind == PinKind::Input) ? node->Outputs : node->Inputs;
  const auto matching_node_pin = std::ranges::find_if(
      node_pins,
      [pin](const auto& node_pin) { return CanCreateLink(pin, &node_pin); });

  if (matching_node_pin == node_pins.end()) {
    return;
  }

  const auto is_link_starts_on_existing_node = pin->Kind == PinKind::Output;
  const auto link = Link{
      .ID = app_->GetNextLinkId(),
      .StartPinID =
          is_link_starts_on_existing_node ? pin->ID : matching_node_pin->ID,
      .EndPinID =
          is_link_starts_on_existing_node ? matching_node_pin->ID : pin->ID,
      .Color = GetIconColor(pin->Type)};

  SpawnLink(link);
}

void NodesAndLinks::SafeToFile(const std::string& file_path) {}

void NodesAndLinks::LoadFromFile(const std::string& file_path) {}
}  // namespace esc