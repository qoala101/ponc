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

NodesAndLinks::NodesAndLinks(std::shared_ptr<AutoIncrementable> auto_object_id)
    : auto_object_id_{(cpp::Expects(auto_object_id != nullptr),
                       std::move(auto_object_id))} {
  cpp::Ensures(auto_object_id_ != nullptr);
}

auto NodesAndLinks::SpawnInputActionNode() -> Node* {
  nodes_.emplace_back(auto_object_id_->GetNext(), "InputAction Fire",
                      ImColor(255, 128, 128));
  nodes_.back().Outputs.emplace_back(auto_object_id_->GetNext(), "",
                                     PinType::Delegate);
  nodes_.back().Outputs.emplace_back(auto_object_id_->GetNext(), "Pressed",
                                     PinType::Flow);
  nodes_.back().Outputs.emplace_back(auto_object_id_->GetNext(), "Released",
                                     PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnBranchNode() -> Node* {
  nodes_.emplace_back(auto_object_id_->GetNext(), "Branch");
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "",
                                    PinType::Flow);
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "Condition",
                                    PinType::Bool);
  nodes_.back().Outputs.emplace_back(auto_object_id_->GetNext(), "True",
                                     PinType::Flow);
  nodes_.back().Outputs.emplace_back(auto_object_id_->GetNext(), "False",
                                     PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnDoNNode() -> Node* {
  nodes_.emplace_back(auto_object_id_->GetNext(), "Do N");
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "Enter",
                                    PinType::Flow);
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "N",
                                    PinType::Int);
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "Reset",
                                    PinType::Flow);
  nodes_.back().Outputs.emplace_back(auto_object_id_->GetNext(), "Exit",
                                     PinType::Flow);
  nodes_.back().Outputs.emplace_back(auto_object_id_->GetNext(), "Counter",
                                     PinType::Int);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnOutputActionNode() -> Node* {
  nodes_.emplace_back(auto_object_id_->GetNext(), "OutputAction");
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "Sample",
                                    PinType::Float);
  nodes_.back().Outputs.emplace_back(auto_object_id_->GetNext(), "Condition",
                                     PinType::Bool);
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "Event",
                                    PinType::Delegate);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnPrintStringNode() -> Node* {
  nodes_.emplace_back(auto_object_id_->GetNext(), "Print String");
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "",
                                    PinType::Flow);
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "In String",
                                    PinType::String);
  nodes_.back().Outputs.emplace_back(auto_object_id_->GetNext(), "",
                                     PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnMessageNode() -> Node* {
  nodes_.emplace_back(auto_object_id_->GetNext(), "", ImColor(128, 195, 248));
  nodes_.back().Type = NodeType::Simple;
  nodes_.back().Outputs.emplace_back(auto_object_id_->GetNext(), "Message",
                                     PinType::String);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnSetTimerNode() -> Node* {
  nodes_.emplace_back(auto_object_id_->GetNext(), "Set Timer",
                      ImColor(128, 195, 248));
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "",
                                    PinType::Flow);
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "Object",
                                    PinType::Object);
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "Function Name",
                                    PinType::Function);
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "Time",
                                    PinType::Float);
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "Looping",
                                    PinType::Bool);
  nodes_.back().Outputs.emplace_back(auto_object_id_->GetNext(), "",
                                     PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnLessNode() -> Node* {
  nodes_.emplace_back(auto_object_id_->GetNext(), "<", ImColor(128, 195, 248));
  nodes_.back().Type = NodeType::Simple;
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "",
                                    PinType::Float);
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "",
                                    PinType::Float);
  nodes_.back().Outputs.emplace_back(auto_object_id_->GetNext(), "",
                                     PinType::Float);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnWeirdNode() -> Node* {
  nodes_.emplace_back(auto_object_id_->GetNext(), "o.O",
                      ImColor(128, 195, 248));
  nodes_.back().Type = NodeType::Simple;
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "",
                                    PinType::Float);
  nodes_.back().Outputs.emplace_back(auto_object_id_->GetNext(), "",
                                     PinType::Float);
  nodes_.back().Outputs.emplace_back(auto_object_id_->GetNext(), "",
                                     PinType::Float);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnTraceByChannelNode() -> Node* {
  nodes_.emplace_back(auto_object_id_->GetNext(),
                      "Single Line Trace by Channel", ImColor(255, 128, 64));
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "",
                                    PinType::Flow);
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "Start",
                                    PinType::Flow);
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "End",
                                    PinType::Int);
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "Trace Channel",
                                    PinType::Float);
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "Trace Complex",
                                    PinType::Bool);
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(),
                                    "Actors to Ignore", PinType::Int);
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(),
                                    "Draw Debug Type", PinType::Bool);
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "Ignore Self",
                                    PinType::Bool);
  nodes_.back().Outputs.emplace_back(auto_object_id_->GetNext(), "",
                                     PinType::Flow);
  nodes_.back().Outputs.emplace_back(auto_object_id_->GetNext(), "Out Hit",
                                     PinType::Float);
  nodes_.back().Outputs.emplace_back(auto_object_id_->GetNext(), "Return Value",
                                     PinType::Bool);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnTreeSequenceNode() -> Node* {
  nodes_.emplace_back(auto_object_id_->GetNext(), "Sequence");
  nodes_.back().Type = NodeType::Tree;
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "",
                                    PinType::Flow);
  nodes_.back().Outputs.emplace_back(auto_object_id_->GetNext(), "",
                                     PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnTreeTaskNode() -> Node* {
  nodes_.emplace_back(auto_object_id_->GetNext(), "Move To");
  nodes_.back().Type = NodeType::Tree;
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "",
                                    PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnTreeTask2Node() -> Node* {
  nodes_.emplace_back(auto_object_id_->GetNext(), "Random Wait");
  nodes_.back().Type = NodeType::Tree;
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "",
                                    PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnComment() -> Node* {
  nodes_.emplace_back(auto_object_id_->GetNext(), "Test Comment");
  nodes_.back().Type = NodeType::Comment;
  nodes_.back().Size = ImVec2(300, 200);

  return &nodes_.back();
}

auto NodesAndLinks::SpawnHoudiniTransformNode() -> Node* {
  nodes_.emplace_back(auto_object_id_->GetNext(), "Transform");
  nodes_.back().Type = NodeType::Houdini;
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "",
                                    PinType::Flow);
  nodes_.back().Outputs.emplace_back(auto_object_id_->GetNext(), "",
                                     PinType::Flow);

  BuildNode(&nodes_.back());

  return &nodes_.back();
}

auto NodesAndLinks::SpawnHoudiniGroupNode() -> Node* {
  nodes_.emplace_back(auto_object_id_->GetNext(), "Group");
  nodes_.back().Type = NodeType::Houdini;
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "",
                                    PinType::Flow);
  nodes_.back().Inputs.emplace_back(auto_object_id_->GetNext(), "",
                                    PinType::Flow);
  nodes_.back().Outputs.emplace_back(auto_object_id_->GetNext(), "",
                                     PinType::Flow);

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