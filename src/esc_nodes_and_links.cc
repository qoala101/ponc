#include "esc_nodes_and_links.h"

#include <crude_json.h>
#include <imgui_node_editor.h>
#include <imgui_node_editor_internal.h>
#include <sys/types.h>

#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "core_float_pin.h"
#include "esc_app.h"
#include "esc_cpp.h"
#include "esc_enums.h"
#include "esc_types.h"
#include "imgui.h"

namespace esc {
NodesAndLinks::NodesAndLinks(std::shared_ptr<App> app)
    : app_{(cpp::Expects(app != nullptr), std::move(app))} {
  cpp::Ensures(app_ != nullptr);
}

auto NodesAndLinks::SpawnInputNode() -> Node* {
  return nodes_
      .emplace_back(std::make_shared<InputNode>(app_->GetIdGenerator()))
      .get();
}

auto NodesAndLinks::SpawnClientNode() -> Node* {
  return nodes_
      .emplace_back(std::make_shared<ClientNode>(app_->GetIdGenerator()))
      .get();
}

auto NodesAndLinks::SpawnCouplerNode() -> Node* {
  return nodes_
      .emplace_back(std::make_shared<CouplerNode>(app_->GetIdGenerator()))
      .get();
}

auto NodesAndLinks::SpawnSplitterNode(int n) -> Node* {
  return nodes_
      .emplace_back(std::make_shared<SplitterNode>(app_->GetIdGenerator(), n))
      .get();
}

auto NodesAndLinks::GetNodes() -> std::vector<std::shared_ptr<Node>>& {
  return nodes_;
}

auto NodesAndLinks::GetLinks() const -> const std::vector<Link>& {
  return links_;
}

auto NodesAndLinks::FindNode(ne::NodeId id) -> Node* {
  for (auto& node : nodes_)
    if (node->GetId() == id) return node.get();

  return nullptr;
}

auto NodesAndLinks::FindPin(ne::PinId id) -> Pin* {
  if (!id) return nullptr;

  for (auto& node : nodes_) {
    for (auto& pin : node->GetInputPins())
      if (pin->GetId() == id) return pin.get();

    for (auto& pin : node->GetOutputPins())
      if (pin->GetId() == id) return pin.get();
  }

  return nullptr;
}

auto NodesAndLinks::FindLink(ne::LinkId id) -> Link* {
  for (auto& link : links_)
    if (link.id == id) return &link;

  return nullptr;
}

auto NodesAndLinks::IsPinLinked(ne::PinId id) -> bool {
  if (!id) return false;

  for (auto& link : links_)
    if (link.start_pin_id == id || link.end_pin_id == id) return true;

  return false;
}

void NodesAndLinks::SpawnLink(const Link& link) { links_.emplace_back(link); }

void NodesAndLinks::EraseLinkWithId(ne::LinkId linkId) {
  auto id = std::find_if(links_.begin(), links_.end(),
                         [linkId](auto& link) { return link.id == linkId; });
  if (id != links_.end()) links_.erase(id);
}

void NodesAndLinks::EraseNodeWithId(ne::NodeId id) {
  const auto node = std::ranges::find_if(
      nodes_, [id](const auto& node) { return node->GetId() == id; });

  if (node != nodes_.end()) {
    nodes_.erase(node);
  }
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

  if (type_name == "Client") {
    return SpawnClientNode();
  }

  if (type_name == "Coupler 1x2") {
    return SpawnCouplerNode();
  }

  if (type_name.starts_with("Splitter 1x")) {
    const auto index_substring = std::string{
        type_name.begin() + static_cast<int>(std::string{"Splitter 1x"}.size()),
        type_name.end()};
    const auto index = std::stoi(index_substring);
    return SpawnSplitterNode(index);
  }

  cpp::Expects(false);
}

auto NodesAndLinks::GetNodeTypeNames() -> std::vector<std::string> {
  auto names = std::vector<std::string>{"Input", "Client", "Coupler 1x2"};

  for (auto i : {2, 4, 8, 16}) {
    names.emplace_back("Splitter 1x" + std::to_string(i));
  }

  return names;
}

void NodesAndLinks::SpawnLinkFromPinToNode(const Pin* pin, const Node* node) {
  const auto* node_pins = (pin->ui_data_.Kind == PinKind::Input)
                              ? &node->GetOutputPins()
                              : &node->GetInputPins();
  const auto matching_node_pin =
      std::ranges::find_if(*node_pins, [pin](const auto& node_pin) {
        return CanCreateLink(pin, node_pin.get());
      });

  if (matching_node_pin == node_pins->end()) {
    return;
  }

  const auto is_link_starts_on_existing_node =
      pin->ui_data_.Kind == PinKind::Output;
  const auto link = Link{.id = app_->GetIdGenerator().GetNext<ne::LinkId>(),
                         .start_pin_id = is_link_starts_on_existing_node
                                             ? pin->GetId()
                                             : (*matching_node_pin)->GetId(),
                         .end_pin_id = is_link_starts_on_existing_node
                                           ? (*matching_node_pin)->GetId()
                                           : pin->GetId()};

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

      node_json["id"] = static_cast<crude_json::number>(node->GetId().Get());
      node_json["name"] = node->ui_data_.name;

      if (auto* coupler_node = dynamic_cast<CouplerNode*>(node.get())) {
        node_json["coupler_percentage_index"] = static_cast<crude_json::number>(
            coupler_node->GetCouplerPercentageIndex());
      }

      const auto pos = ne::GetNodePosition(node->GetId());

      node_json["pos_x"] = pos.x;
      node_json["pos_y"] = pos.y;

      const auto size = ne::GetNodeSize(node->GetId());

      node_json["size_x"] = size.x;
      node_json["size_y"] = size.y;

      {
        auto& input_pins_json = node_json["input_pins"];
        auto input_pin_index = 0;

        for (const auto& input_pin : node->GetInputPins()) {
          auto& input_pin_json = input_pins_json[input_pin_index++];
          input_pin_json["id"] =
              static_cast<crude_json::number>(input_pin->GetId().Get());

          if (auto* float_pin = dynamic_cast<FloatPin*>(input_pin.get())) {
            input_pin_json["value"] = float_pin->GetValue();
          }
        }
      }

      {
        auto& output_pins_json = node_json["output_pins"];
        auto output_pin_index = 0;

        for (const auto& output_pin : node->GetOutputPins()) {
          auto& output_pin_json = output_pins_json[output_pin_index++];
          output_pin_json["id"] =
              static_cast<crude_json::number>(output_pin->GetId().Get());

          if (auto* float_pin = dynamic_cast<FloatPin*>(output_pin.get())) {
            output_pin_json["value"] = float_pin->GetValue();
          }
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

      link_json["id"] = static_cast<crude_json::number>(link.id.Get());
      link_json["start_pin_id"] =
          static_cast<crude_json::number>(link.start_pin_id.Get());
      link_json["end_pin_id"] =
          static_cast<crude_json::number>(link.end_pin_id.Get());
    }
  }

  std::cout << "save:" << json.dump() << "\n";
  json.save(file_path);
}

void NodesAndLinks::DeleteAll() {
  for (const auto& link : links_) {
    ne::DeleteLink(link.id);
  }

  links_.clear();

  for (const auto& node : nodes_) {
    ne::DeleteNode(node->GetId());
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

    if (auto* coupler_node = dynamic_cast<CouplerNode*>(node)) {
      coupler_node->SetCouplerPercentageIndex(static_cast<int>(
          node_json["coupler_percentage_index"].get<crude_json::number>()));
    }

    {
      const auto& input_pins = node_json["input_pins"];

      for (auto i = 0; i < static_cast<int>(node->GetInputPins().size()); ++i) {
        const auto& input_pin_json = input_pins[i];
        node->GetInputPins()[i]->SetId(static_cast<uint64_t>(
            input_pin_json["id"].get<crude_json::number>()));

        if (auto* float_pin =
                dynamic_cast<FloatPin*>(node->GetInputPins()[i].get())) {
          float_pin->SetValue(static_cast<float>(
              input_pin_json["value"].get<crude_json::number>()));
        }
      }
    }

    {
      const auto& output_pins = node_json["output_pins"];

      for (auto i = 0; i < static_cast<int>(node->GetOutputPins().size());
           ++i) {
        const auto& output_pin_json = output_pins[i];
        node->GetOutputPins()[i]->SetId(static_cast<uint64_t>(
            output_pin_json["id"].get<crude_json::number>()));

        if (auto* float_pin =
                dynamic_cast<FloatPin*>(node->GetOutputPins()[i].get())) {
          float_pin->SetValue(static_cast<float>(
              output_pin_json["value"].get<crude_json::number>()));
        }
      }
    }

    ne::SetNodePosition(
        node->GetId(),
        {static_cast<float>(node_json["pos_x"].get<crude_json::number>()),
         static_cast<float>(node_json["pos_y"].get<crude_json::number>())});
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
}

void NodesAndLinks::OnFrame() {
  UpdateNodePointerOnPins();
  UpdatePinValues();
}

void NodesAndLinks::UpdateNodePointerOnPins() {
  for (auto& node : nodes_) {
    for (auto& input : node->GetInputPins()) {
      input->ui_data_.node = node.get();
    }

    for (auto& output : node->GetOutputPins()) {
      output->ui_data_.node = node.get();
    }
  }
}

void NodesAndLinks::ClearAllValuesExceptInput() {
  const auto& coupler_percentage_values = GetCouplerPercentageValues();

  for (auto& node : nodes_) {
    for (const auto& pins : {&node->GetInputPins(), &node->GetOutputPins()}) {
      for (auto& pin : *pins) {
        if (!pin->ui_data_.editable) {
          if (auto* float_pin = dynamic_cast<FloatPin*>(pin.get())) {
            float_pin->SetValue(0);
          }
        }
      }
    }

    if (node->ui_data_.name.starts_with("Splitter 1x")) {
      const auto index_substring =
          std::string{node->ui_data_.name.begin() +
                          static_cast<int>(std::string{"Splitter 1x"}.size()),
                      node->ui_data_.name.end()};
      const auto index = std::stoi(index_substring);

      static const auto kSplitterValuesMap =
          std::map<int, float>{{2, 4.3}, {4, 7.4}, {8, 10.7}, {16, 13.9}};

      dynamic_cast<FloatPin*>(node->GetInputPins()[1].get())
          ->SetValue(kSplitterValuesMap.at(index));
    } else if (auto* coupler_node = dynamic_cast<CouplerNode*>(node.get())) {
      const auto& values =
          coupler_percentage_values[coupler_node->GetCouplerPercentageIndex()];

      dynamic_cast<FloatPin*>(node->GetInputPins()[1].get())
          ->SetValue(values.first);
      dynamic_cast<FloatPin*>(node->GetInputPins()[2].get())
          ->SetValue(values.second);
    }
  }
}

void NodesAndLinks::UpdatePinValues() {
  ClearAllValuesExceptInput();

  auto input_nodes = std::vector<Node*>{};

  for (auto& node : nodes_) {
    if (node->ui_data_.name == "Input") {
      input_nodes.emplace_back(node.get());
    }
  }

  auto visited_nodes =
      std::unordered_set<Node*>{input_nodes.begin(), input_nodes.end()};

  while (!input_nodes.empty()) {
    auto next_input_nodes = std::unordered_set<Node*>{};

    for (auto* input_node : input_nodes) {
      if (!visited_nodes.contains(input_node)) {
        visited_nodes.emplace(input_node);

        if (input_node->ui_data_.name.starts_with("Splitter 1x")) {
          const auto splitter_value =
              dynamic_cast<FloatPin*>(input_node->GetInputPins()[1].get())
                  ->GetValue();

          for (auto& output_pin : input_node->GetOutputPins()) {
            if (auto* float_pin = dynamic_cast<FloatPin*>(output_pin.get())) {
              float_pin->SetValue(float_pin->GetValue() - splitter_value);
            }
          }
        } else if (input_node->ui_data_.name == "Coupler 1x2") {
          dynamic_cast<FloatPin*>(input_node->GetOutputPins()[1].get())
              ->SetValue(
                  dynamic_cast<FloatPin*>(input_node->GetOutputPins()[1].get())
                      ->GetValue() -
                  dynamic_cast<FloatPin*>(input_node->GetInputPins()[1].get())
                      ->GetValue());
          dynamic_cast<FloatPin*>(input_node->GetOutputPins()[2].get())
              ->SetValue(
                  dynamic_cast<FloatPin*>(input_node->GetOutputPins()[2].get())
                      ->GetValue() -
                  dynamic_cast<FloatPin*>(input_node->GetInputPins()[2].get())
                      ->GetValue());
        }
      }

      for (auto& input_node_output_pin : input_node->GetOutputPins()) {
        if (auto* float_input_node_output_pin =
                dynamic_cast<FloatPin*>(input_node_output_pin.get())) {
          for (auto& link : links_) {
            if (link.start_pin_id == input_node_output_pin->GetId()) {
              auto* end_pin = FindPin(link.end_pin_id);

              if ((end_pin == nullptr) || (end_pin->ui_data_.node == nullptr)) {
                continue;
              }

              if (auto* float_end_pin = dynamic_cast<FloatPin*>(end_pin)) {
                float_end_pin->SetValue(
                    float_end_pin->GetValue() +
                    float_input_node_output_pin->GetValue());

                for (auto& output_pin :
                     end_pin->ui_data_.node->GetOutputPins()) {
                  if (auto* float_output_pin =
                          dynamic_cast<FloatPin*>(output_pin.get())) {
                    float_output_pin->SetValue(
                        float_output_pin->GetValue() +
                        float_input_node_output_pin->GetValue());
                  }

                  next_input_nodes.emplace(end_pin->ui_data_.node);
                }
              }
            }
          }
        }
      }
    }

    input_nodes.assign(next_input_nodes.begin(), next_input_nodes.end());
  }
}
}  // namespace esc