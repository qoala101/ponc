#include "esc_nodes_and_links.h"

#include <imgui_node_editor.h>

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

auto NodesAndLinks::SpawnInputActionNode() -> Node* {
  nodes_.emplace_back(app_->GetNextObjectId(), "InputAction Fire",
                      ImColor(255, 128, 128));
  nodes_.back().Outputs.emplace_back(app_->GetNextObjectId(), "Pressed",
                                     PinType::Flow);
  nodes_.back().Outputs.emplace_back(app_->GetNextObjectId(), "Released",
                                     PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnBranchNode() -> Node* {
  nodes_.emplace_back(app_->GetNextObjectId(), "Branch");
  nodes_.back().Inputs.emplace_back(app_->GetNextObjectId(), "", PinType::Flow);
  nodes_.back().Outputs.emplace_back(app_->GetNextObjectId(), "True",
                                     PinType::Flow);
  nodes_.back().Outputs.emplace_back(app_->GetNextObjectId(), "False",
                                     PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnDoNNode() -> Node* {
  nodes_.emplace_back(app_->GetNextObjectId(), "Do N");
  nodes_.back().Inputs.emplace_back(app_->GetNextObjectId(), "Enter",
                                    PinType::Flow);
  nodes_.back().Inputs.emplace_back(app_->GetNextObjectId(), "Reset",
                                    PinType::Flow);
  nodes_.back().Outputs.emplace_back(app_->GetNextObjectId(), "Exit",
                                     PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnPrintStringNode() -> Node* {
  nodes_.emplace_back(app_->GetNextObjectId(), "Print String");
  nodes_.back().Inputs.emplace_back(app_->GetNextObjectId(), "", PinType::Flow);
  nodes_.back().Outputs.emplace_back(app_->GetNextObjectId(), "",
                                     PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnComment() -> Node* {
  nodes_.emplace_back(app_->GetNextObjectId(), "Test Comment");
  nodes_.back().Type = NodeType::Comment;
  nodes_.back().Size = ImVec2(300, 200);

  return &nodes_.back();
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
  if (type_name == "Input Action") {
    return SpawnInputActionNode();
  }

  if (type_name == "Branch") {
    return SpawnBranchNode();
  }

  if (type_name == "Do N") {
    return SpawnDoNNode();
  }

  if (type_name == "Print String") {
    return SpawnPrintStringNode();
  }

  if (type_name == "Comment") {
    return SpawnComment();
  }
}

auto NodesAndLinks::GetNodeTypeNames() -> std::vector<std::string> {
  return {"Input Action", "Branch", "Do N", "Print String", "Comment"};
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
}  // namespace esc