#include "esc_nodes_and_links.h"

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

NodesAndLinks::NodesAndLinks(std::function<int()> id_provider)
    : object_id_provider_{
          (cpp::Expects(bool{id_provider}), std::move(id_provider))} {
  cpp::Ensures(bool{object_id_provider_});
}

auto NodesAndLinks::SpawnInputActionNode() -> Node* {
  nodes_.emplace_back(object_id_provider_(), "InputAction Fire",
                      ImColor(255, 128, 128));
  nodes_.back().Outputs.emplace_back(object_id_provider_(), "",
                                     PinType::Delegate);
  nodes_.back().Outputs.emplace_back(object_id_provider_(), "Pressed",
                                     PinType::Flow);
  nodes_.back().Outputs.emplace_back(object_id_provider_(), "Released",
                                     PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnBranchNode() -> Node* {
  nodes_.emplace_back(object_id_provider_(), "Branch");
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "", PinType::Flow);
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "Condition",
                                    PinType::Bool);
  nodes_.back().Outputs.emplace_back(object_id_provider_(), "True",
                                     PinType::Flow);
  nodes_.back().Outputs.emplace_back(object_id_provider_(), "False",
                                     PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnDoNNode() -> Node* {
  nodes_.emplace_back(object_id_provider_(), "Do N");
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "Enter",
                                    PinType::Flow);
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "N", PinType::Int);
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "Reset",
                                    PinType::Flow);
  nodes_.back().Outputs.emplace_back(object_id_provider_(), "Exit",
                                     PinType::Flow);
  nodes_.back().Outputs.emplace_back(object_id_provider_(), "Counter",
                                     PinType::Int);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnOutputActionNode() -> Node* {
  nodes_.emplace_back(object_id_provider_(), "OutputAction");
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "Sample",
                                    PinType::Float);
  nodes_.back().Outputs.emplace_back(object_id_provider_(), "Condition",
                                     PinType::Bool);
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "Event",
                                    PinType::Delegate);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnPrintStringNode() -> Node* {
  nodes_.emplace_back(object_id_provider_(), "Print String");
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "", PinType::Flow);
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "In String",
                                    PinType::String);
  nodes_.back().Outputs.emplace_back(object_id_provider_(), "", PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnMessageNode() -> Node* {
  nodes_.emplace_back(object_id_provider_(), "", ImColor(128, 195, 248));
  nodes_.back().Type = NodeType::Simple;
  nodes_.back().Outputs.emplace_back(object_id_provider_(), "Message",
                                     PinType::String);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnSetTimerNode() -> Node* {
  nodes_.emplace_back(object_id_provider_(), "Set Timer",
                      ImColor(128, 195, 248));
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "", PinType::Flow);
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "Object",
                                    PinType::Object);
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "Function Name",
                                    PinType::Function);
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "Time",
                                    PinType::Float);
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "Looping",
                                    PinType::Bool);
  nodes_.back().Outputs.emplace_back(object_id_provider_(), "", PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnLessNode() -> Node* {
  nodes_.emplace_back(object_id_provider_(), "<", ImColor(128, 195, 248));
  nodes_.back().Type = NodeType::Simple;
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "", PinType::Float);
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "", PinType::Float);
  nodes_.back().Outputs.emplace_back(object_id_provider_(), "", PinType::Float);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnWeirdNode() -> Node* {
  nodes_.emplace_back(object_id_provider_(), "o.O", ImColor(128, 195, 248));
  nodes_.back().Type = NodeType::Simple;
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "", PinType::Float);
  nodes_.back().Outputs.emplace_back(object_id_provider_(), "", PinType::Float);
  nodes_.back().Outputs.emplace_back(object_id_provider_(), "", PinType::Float);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnTraceByChannelNode() -> Node* {
  nodes_.emplace_back(object_id_provider_(), "Single Line Trace by Channel",
                      ImColor(255, 128, 64));
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "", PinType::Flow);
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "Start",
                                    PinType::Flow);
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "End", PinType::Int);
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "Trace Channel",
                                    PinType::Float);
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "Trace Complex",
                                    PinType::Bool);
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "Actors to Ignore",
                                    PinType::Int);
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "Draw Debug Type",
                                    PinType::Bool);
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "Ignore Self",
                                    PinType::Bool);
  nodes_.back().Outputs.emplace_back(object_id_provider_(), "", PinType::Flow);
  nodes_.back().Outputs.emplace_back(object_id_provider_(), "Out Hit",
                                     PinType::Float);
  nodes_.back().Outputs.emplace_back(object_id_provider_(), "Return Value",
                                     PinType::Bool);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnTreeSequenceNode() -> Node* {
  nodes_.emplace_back(object_id_provider_(), "Sequence");
  nodes_.back().Type = NodeType::Tree;
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "", PinType::Flow);
  nodes_.back().Outputs.emplace_back(object_id_provider_(), "", PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnTreeTaskNode() -> Node* {
  nodes_.emplace_back(object_id_provider_(), "Move To");
  nodes_.back().Type = NodeType::Tree;
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "", PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnTreeTask2Node() -> Node* {
  nodes_.emplace_back(object_id_provider_(), "Random Wait");
  nodes_.back().Type = NodeType::Tree;
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "", PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnComment() -> Node* {
  nodes_.emplace_back(object_id_provider_(), "Test Comment");
  nodes_.back().Type = NodeType::Comment;
  nodes_.back().Size = ImVec2(300, 200);

  return &nodes_.back();
}

auto NodesAndLinks::SpawnHoudiniTransformNode() -> Node* {
  nodes_.emplace_back(object_id_provider_(), "Transform");
  nodes_.back().Type = NodeType::Houdini;
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "", PinType::Flow);
  nodes_.back().Outputs.emplace_back(object_id_provider_(), "", PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnHoudiniGroupNode() -> Node* {
  nodes_.emplace_back(object_id_provider_(), "Group");
  nodes_.back().Type = NodeType::Houdini;
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "", PinType::Flow);
  nodes_.back().Inputs.emplace_back(object_id_provider_(), "", PinType::Flow);
  nodes_.back().Outputs.emplace_back(object_id_provider_(), "", PinType::Flow);

  BuildNode(&nodes_.back());

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
}  // namespace esc