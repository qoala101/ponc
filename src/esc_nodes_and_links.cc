#include "esc_nodes_and_links.h"

#include <crude_json.h>
#include <imgui_node_editor.h>
#include <imgui_node_editor_internal.h>
#include <sys/types.h>

#include <iostream>
#include <string>

#include "esc_app.h"
#include "esc_cpp.h"
#include "imgui.h"

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

  auto& output =
      node.Outputs.emplace_back(app_->GetNextObjectId(), "", PinType::Flow);
  output.editable = true;

  BuildNode(&node);
  return &node;
}

auto NodesAndLinks::SpawnDividerNode() -> Node* {
  auto& node = nodes_.emplace_back(app_->GetNextObjectId(), "Divider",
                                   ImColor{127, 127, 127});

  node.Inputs.emplace_back(app_->GetNextObjectId(), "", PinType::Flow);
  node.Inputs.emplace_back(app_->GetNextObjectId(), "", PinType::Float);
  node.Inputs.emplace_back(app_->GetNextObjectId(), "", PinType::Float);

  node.Outputs.emplace_back(app_->GetNextObjectId(), "", PinType::Empty);
  auto* output =
      &node.Outputs.emplace_back(app_->GetNextObjectId(), "", PinType::Flow);
  output->editable = true;
  output =
      &node.Outputs.emplace_back(app_->GetNextObjectId(), "", PinType::Flow);
  output->editable = true;

  BuildNode(&node);
  return &node;
}

auto NodesAndLinks::Spawn1ToNNode(int n) -> Node* {
  const auto n_string = std::to_string(n);
  const auto node_name = "1/" + n_string;

  auto& node = nodes_.emplace_back(app_->GetNextObjectId(), node_name.c_str(),
                                   ImColor{127 / n, 127 / n, 255});

  node.Inputs.emplace_back(app_->GetNextObjectId(), "", PinType::Flow);

  auto& input =
      node.Inputs.emplace_back(app_->GetNextObjectId(), "", PinType::Float);
  input.editable = true;

  for (auto i = 0; i < n; ++i) {
    node.Outputs.emplace_back(app_->GetNextObjectId(), "", PinType::Flow);
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

  if (type_name == "Divider") {
    return SpawnDividerNode();
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
  return {"Input", "Divider", "1/2", "1/4", "1/8", "1/16", "Client", "Comment"};
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

void NodesAndLinks::SafeToFile(const std::string& file_path) {
  auto json = crude_json::value{};

  {
    json["nodes_size"] = static_cast<crude_json::number>(nodes_.size());
    auto& nodes_json = json["nodes"];
    auto node_index = 0;

    for (const auto& node : nodes_) {
      auto& node_json = nodes_json[node_index++];

      node_json["id"] = static_cast<crude_json::number>(node.ID.Get());
      node_json["name"] = node.Name;

      const auto pos = ne::GetNodePosition(node.ID);

      node_json["pos_x"] = pos.x;
      node_json["pos_y"] = pos.y;

      const auto size = ne::GetNodeSize(node.ID);

      node_json["size_x"] = size.x;
      node_json["size_y"] = size.y;

      {
        auto& input_pins_json = node_json["input_pin_ids"];
        auto input_pin_index = 0;

        for (const auto& input_pin : node.Inputs) {
          input_pins_json[input_pin_index++] =
              static_cast<crude_json::number>(input_pin.ID.Get());
        }
      }

      {
        auto& output_pins_json = node_json["output_pin_ids"];
        auto output_pin_index = 0;

        for (const auto& output_pin : node.Outputs) {
          output_pins_json[output_pin_index++] =
              static_cast<crude_json::number>(output_pin.ID.Get());
        }
      }
    }
  }

  {
    json["links_size"] = static_cast<crude_json::number>(links_.size());
    auto& links_json = json["links"];
    auto link_index = 0;

    for (const auto& link : links_) {
      auto& link_json = links_json[link_index++];

      link_json["id"] = static_cast<crude_json::number>(link.ID.Get());
      link_json["start_pin_id"] =
          static_cast<crude_json::number>(link.StartPinID.Get());
      link_json["end_pin_id"] =
          static_cast<crude_json::number>(link.EndPinID.Get());
    }
  }

  std::cout << "save:" << json.dump() << "\n";
  json.save(file_path);
}

void NodesAndLinks::DeleteAll() {
  for (const auto& link : links_) {
    ne::DeleteLink(link.ID);
  }

  links_.clear();

  for (const auto& node : nodes_) {
    ne::DeleteNode(node.ID);
  }

  nodes_.clear();
}

void NodesAndLinks::LoadFromFile(const std::string& file_path) {
  DeleteAll();

  const auto json = crude_json::value::load(file_path).first;
  const auto nodes_size = json["nodes_size"].get<crude_json::number>();
  const auto& nodes_json = json["nodes"];

  for (auto i = 0; i < nodes_size; ++i) {
    const auto& node_json = nodes_json[i];

    auto* node =
        SpawnNodeByTypeName(node_json["name"].get<crude_json::string>());

    {
      const auto& input_pin_ids = node_json["input_pin_ids"];

      for (auto i = 0; i < static_cast<int>(node->Inputs.size()); ++i) {
        node->Inputs[i].ID =
            static_cast<uint64_t>(input_pin_ids[i].get<crude_json::number>());
      }
    }

    {
      const auto& output_pin_ids = node_json["output_pin_ids"];

      for (auto i = 0; i < static_cast<int>(node->Outputs.size()); ++i) {
        node->Outputs[i].ID =
            static_cast<uint64_t>(output_pin_ids[i].get<crude_json::number>());
      }
    }

    ne::SetNodePosition(
        node->ID,
        {static_cast<float>(node_json["pos_x"].get<crude_json::number>()),
         static_cast<float>(node_json["pos_y"].get<crude_json::number>())});

    if (node->Name == "Comment") {
      ne::SetGroupSize(
          node->ID,
          {static_cast<float>(node_json["size_x"].get<crude_json::number>()),
           static_cast<float>(node_json["size_y"].get<crude_json::number>())});
    }
  }

  const auto links_size = json["links_size"].get<crude_json::number>();
  const auto& links_json = json["links"];

  for (auto i = 0; i < links_size; ++i) {
    const auto& link_json = links_json[i];

    SpawnLink({static_cast<uint64_t>(link_json["id"].get<crude_json::number>()),
               static_cast<uint64_t>(
                   link_json["start_pin_id"].get<crude_json::number>()),
               static_cast<uint64_t>(
                   link_json["end_pin_id"].get<crude_json::number>())});
  }

  BuildNodes();
}

//   cpp::Expects(nodes_and_links_.has_value());

//   auto* node = static_cast<Node*>(nullptr);

//   node = nodes_and_links_->SpawnInputActionNode();
//   ne::SetNodePosition(node->ID, ImVec2(-252, 220));

//   node = nodes_and_links_->SpawnBranchNode();
//   ne::SetNodePosition(node->ID, ImVec2(-300, 351));
//   node = nodes_and_links_->SpawnDoNNode();
//   ne::SetNodePosition(node->ID, ImVec2(-238, 504));

//   node = nodes_and_links_->SpawnPrintStringNode();
//   ne::SetNodePosition(node->ID, ImVec2(-69, 652));

//   node = nodes_and_links_->SpawnComment();
//   ne::SetNodePosition(node->ID, ImVec2(800, 224));
//   ne::SetGroupSize(node->ID, ImVec2(640, 400));

//   nodes_and_links_->BuildNodes();
// }
// // vh: norm
// void App::AddInitialLinks() {
//   cpp::Expects(nodes_and_links_.has_value());

//   nodes_and_links_->SpawnLink({GetNextLinkId(),
//                                nodes_and_links_->GetNodes()[0].Outputs[0].ID,
//                                nodes_and_links_->GetNodes()[1].Inputs[0].ID});
//   nodes_and_links_->SpawnLink({GetNextLinkId(),
//                                nodes_and_links_->GetNodes()[0].Outputs[1].ID,
//                                nodes_and_links_->GetNodes()[2].Inputs[0].ID});
//   nodes_and_links_->SpawnLink({GetNextLinkId(),
//                                nodes_and_links_->GetNodes()[2].Outputs[0].ID,
//                                nodes_and_links_->GetNodes()[3].Inputs[0].ID});
// }
}  // namespace esc