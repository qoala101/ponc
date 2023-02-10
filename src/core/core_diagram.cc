#include "core_diagram.h"

#include <crude_json.h>
#include <imgui_node_editor.h>
#include <imgui_node_editor_internal.h>
#include <sys/types.h>

#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "core_float_pin.h"
#include "core_link.h"
#include "esc_app.h"
#include "esc_cpp.h"
#include "esc_enums.h"
#include "esc_types.h"
#include "imgui.h"

namespace esc::core {
Diagram::Diagram(std::vector<std::shared_ptr<INodeFactory>> node_factories)
    : node_factories_{std::move(node_factories)} {
}

auto Diagram::GetNodes() const -> const std::vector<std::shared_ptr<INode>>& {
  return nodes_;
}

auto Diagram::GetLinks() const -> const std::vector<Link>& { return links_; }

auto Diagram::EmplaceNode(std::shared_ptr<INode> node) -> INode& {
  return *nodes_.emplace_back(std::move(node));
}

auto Diagram::FindNode(ne::NodeId id) -> INode* {
  for (auto& node : nodes_)
    if (node->GetId() == id) return node.get();

  return nullptr;
}

auto Diagram::FindPin(ne::PinId id) -> IPin* {
  if (!id) return nullptr;

  for (auto& node : nodes_) {
    for (auto& pin : node->GetInputPins())
      if (pin->GetId() == id) return pin.get();

    for (auto& pin : node->GetOutputPins())
      if (pin->GetId() == id) return pin.get();
  }

  return nullptr;
}

auto Diagram::FindLink(ne::LinkId id) -> Link* {
  for (auto& link : links_)
    if (link.id == id) return &link;

  return nullptr;
}

auto Diagram::EmplaceLink(const Link& link) -> Link & { links_.emplace_back(link); }

void Diagram::EraseLink(ne::LinkId linkId) {
  auto id = std::find_if(links_.begin(), links_.end(),
                         [linkId](auto& link) { return link.id == linkId; });
  if (id != links_.end()) links_.erase(id);
}

void Diagram::EraseNode(ne::NodeId id) {
  const auto node = std::ranges::find_if(
      nodes_, [id](const auto& node) { return node->GetId() == id; });

  if (node != nodes_.end()) {
    nodes_.erase(node);
  }
}

auto Diagram::GetSelectedNodeIds() -> std::vector<ne::NodeId> {
  const auto num_selected_objects = ne::GetSelectedObjectCount();

  auto selected_ids = std::vector<ne::NodeId>{};
  selected_ids.resize(num_selected_objects);

  const auto num_selected_nodes = ne::GetSelectedNodes(
      selected_ids.data(), static_cast<int>(selected_ids.size()));
  selected_ids.resize(num_selected_nodes);

  return selected_ids;
}

auto Diagram::GetSelectedLinkIds() -> std::vector<ne::LinkId> {
  const auto num_selected_objects = ne::GetSelectedObjectCount();

  auto selected_ids = std::vector<ne::LinkId>{};
  selected_ids.resize(num_selected_objects);

  const auto num_selected_links = ne::GetSelectedLinks(
      selected_ids.data(), static_cast<int>(selected_ids.size()));
  selected_ids.resize(num_selected_links);

  return selected_ids;
}

auto Diagram::GetNodeFactories() const
    -> const std::vector<std::shared_ptr<INodeFactory>>& {
  return node_factories_;
}

void Diagram::SaveToFile(const std::string& file_path) {
  // auto json = crude_json::value{};

  // {
  //   json["nodes_size"] = static_cast<crude_json::number>(nodes_.size());
  //   auto& nodes_json = json["nodes"];
  //   auto node_index = 0;

  //   for (const auto& node : nodes_) {
  //     auto& node_json = nodes_json[node_index++];

  //     node_json["id"] = static_cast<crude_json::number>(node->GetId().Get());

  //     if (auto* coupler_node = dynamic_cast<CouplerNode*>(node.get())) {
  //       node_json["coupler_percentage_index"] =
  //       static_cast<crude_json::number>(
  //           coupler_node->GetCouplerPercentageIndex());
  //     }

  //     const auto pos = ne::GetNodePosition(node->GetId());

  //     node_json["pos_x"] = pos.x;
  //     node_json["pos_y"] = pos.y;

  //     const auto size = ne::GetNodeSize(node->GetId());

  //     node_json["size_x"] = size.x;
  //     node_json["size_y"] = size.y;

  //     {
  //       auto& input_pins_json = node_json["input_pins"];
  //       auto input_pin_index = 0;

  //       for (const auto& input_pin : node->GetInputPins()) {
  //         auto& input_pin_json = input_pins_json[input_pin_index++];
  //         input_pin_json["id"] =
  //             static_cast<crude_json::number>(input_pin->GetId().Get());

  //         if (auto* float_pin = dynamic_cast<FloatPin*>(input_pin.get())) {
  //           input_pin_json["value"] = float_pin->GetValue();
  //         }
  //       }
  //     }

  //     {
  //       auto& output_pins_json = node_json["output_pins"];
  //       auto output_pin_index = 0;

  //       for (const auto& output_pin : node->GetOutputPins()) {
  //         auto& output_pin_json = output_pins_json[output_pin_index++];
  //         output_pin_json["id"] =
  //             static_cast<crude_json::number>(output_pin->GetId().Get());

  //         if (auto* float_pin = dynamic_cast<FloatPin*>(output_pin.get())) {
  //           output_pin_json["value"] = float_pin->GetValue();
  //         }
  //       }
  //     }
  //   }
  // }

  // {
  //   json["links_size"] = static_cast<crude_json::number>(links_.size());
  //   auto& links_json = json["links"];
  //   auto link_index = 0;

  //   for (const auto& link : links_) {
  //     auto& link_json = links_json[link_index++];

  //     link_json["id"] = static_cast<crude_json::number>(link.id.Get());
  //     link_json["start_pin_id"] =
  //         static_cast<crude_json::number>(link.start_pin_id.Get());
  //     link_json["end_pin_id"] =
  //         static_cast<crude_json::number>(link.end_pin_id.Get());
  //   }
  // }

  // std::cout << "save:" << json.dump() << "\n";
  // json.save(file_path);
}

void Diagram::Clear() {
  for (const auto& link : links_) {
    ne::DeleteLink(link.id);
  }

  links_.clear();

  for (const auto& node : nodes_) {
    ne::DeleteNode(node->GetId());
  }

  nodes_.clear();
}

void Diagram::LoadFromFile(const std::string& file_path) {
  // DeleteAll();

  // const auto json = crude_json::value::load(file_path).first;
  // const auto nodes_size = json["nodes_size"].get<crude_json::number>();
  // const auto& nodes_json = json["nodes"];

  // for (auto i = 0; i < nodes_size; ++i) {
  //   const auto& node_json = nodes_json[i];

  //   auto* node =
  //       SpawnNodeByTypeName(node_json["name"].get<crude_json::string>());

  //   if (auto* coupler_node = dynamic_cast<CouplerNode*>(node)) {
  //     coupler_node->SetCouplerPercentageIndex(static_cast<int>(
  //         node_json["coupler_percentage_index"].get<crude_json::number>()));
  //   }

  //   {
  //     const auto& input_pins = node_json["input_pins"];

  //     for (auto i = 0; i < static_cast<int>(node->GetInputPins().size());
  //     ++i) {
  //       const auto& input_pin_json = input_pins[i];
  //       node->GetInputPins()[i]->SetId(static_cast<uint64_t>(
  //           input_pin_json["id"].get<crude_json::number>()));

  //       if (auto* float_pin =
  //               dynamic_cast<FloatPin*>(node->GetInputPins()[i].get())) {
  //         float_pin->SetValue(static_cast<float>(
  //             input_pin_json["value"].get<crude_json::number>()));
  //       }
  //     }
  //   }

  //   {
  //     const auto& output_pins = node_json["output_pins"];

  //     for (auto i = 0; i < static_cast<int>(node->GetOutputPins().size());
  //          ++i) {
  //       const auto& output_pin_json = output_pins[i];
  //       node->GetOutputPins()[i]->SetId(static_cast<uint64_t>(
  //           output_pin_json["id"].get<crude_json::number>()));

  //       if (auto* float_pin =
  //               dynamic_cast<FloatPin*>(node->GetOutputPins()[i].get())) {
  //         float_pin->SetValue(static_cast<float>(
  //             output_pin_json["value"].get<crude_json::number>()));
  //       }
  //     }
  //   }

  //   ne::SetNodePosition(
  //       node->GetId(),
  //       {static_cast<float>(node_json["pos_x"].get<crude_json::number>()),
  //        static_cast<float>(node_json["pos_y"].get<crude_json::number>())});
  // }

  // const auto links_size = json["links_size"].get<crude_json::number>();
  // const auto& links_json = json["links"];

  // for (auto i = 0; i < links_size; ++i) {
  //   const auto& link_json = links_json[i];

  //   AddLink({static_cast<uint64_t>(link_json["id"].get<crude_json::number>()),
  //              static_cast<uint64_t>(
  //                  link_json["start_pin_id"].get<crude_json::number>()),
  //              static_cast<uint64_t>(
  //                  link_json["end_pin_id"].get<crude_json::number>())});
  // }
}

void Diagram::OnFrame() {
  UpdateNodePointerOnPins();
  UpdatePinValues();
}

void Diagram::UpdateNodePointerOnPins() {
  for (auto& node : nodes_) {
    for (auto& input : node->GetInputPins()) {
      input->ui_data_.node = node.get();
    }

    for (auto& output : node->GetOutputPins()) {
      output->ui_data_.node = node.get();
    }
  }
}

void Diagram::ClearAllValuesExceptInput() {
  // const auto& coupler_percentage_values = GetCouplerPercentageValues();

  // for (auto& node : nodes_) {
  //   for (const auto& pins : {&node->GetInputPins(), &node->GetOutputPins()})
  //   {
  //     for (auto& pin : *pins) {
  //       if (!pin->ui_data_.editable) {
  //         if (auto* float_pin = dynamic_cast<FloatPin*>(pin.get())) {
  //           float_pin->SetValue(0);
  //         }
  //       }
  //     }
  //   }

  //   if (auto* splitter_node = dynamic_cast<SplitterNode*>(node.get())) {
  //     const auto index =
  //         static_cast<int>(splitter_node->GetOutputPins().size());

  //     static const auto kSplitterValuesMap =
  //         std::map<int, float>{{2, 4.3}, {4, 7.4}, {8, 10.7}, {16, 13.9}};

  //     dynamic_cast<FloatPin*>(node->GetInputPins()[1].get())
  //         ->SetValue(kSplitterValuesMap.at(index));
  //   } else if (auto* coupler_node = dynamic_cast<CouplerNode*>(node.get())) {
  //     const auto& values =
  //         coupler_percentage_values[coupler_node->GetCouplerPercentageIndex()];

  //     dynamic_cast<FloatPin*>(node->GetInputPins()[1].get())
  //         ->SetValue(values.first);
  //     dynamic_cast<FloatPin*>(node->GetInputPins()[2].get())
  //         ->SetValue(values.second);
  //   }
  // }
}

void Diagram::UpdatePinValues() {
  // ClearAllValuesExceptInput();

  // auto input_nodes = std::vector<Node*>{};

  // for (auto& node : nodes_) {
  //   if (dynamic_cast<InputNode*>(node.get()) != nullptr) {
  //     input_nodes.emplace_back(node.get());
  //   }
  // }

  // auto visited_nodes =
  //     std::unordered_set<Node*>{input_nodes.begin(), input_nodes.end()};

  // while (!input_nodes.empty()) {
  //   auto next_input_nodes = std::unordered_set<Node*>{};

  //   for (auto* input_node : input_nodes) {
  //     if (!visited_nodes.contains(input_node)) {
  //       visited_nodes.emplace(input_node);

  //       if (dynamic_cast<SplitterNode*>(input_node) != nullptr) {
  //         const auto splitter_value =
  //             dynamic_cast<FloatPin*>(input_node->GetInputPins()[1].get())
  //                 ->GetValue();

  //         for (auto& output_pin : input_node->GetOutputPins()) {
  //           if (auto* float_pin = dynamic_cast<FloatPin*>(output_pin.get()))
  //           {
  //             float_pin->SetValue(float_pin->GetValue() - splitter_value);
  //           }
  //         }
  //       } else if (dynamic_cast<CouplerNode*>(input_node) != nullptr) {
  //         dynamic_cast<FloatPin*>(input_node->GetOutputPins()[1].get())
  //             ->SetValue(
  //                 dynamic_cast<FloatPin*>(input_node->GetOutputPins()[1].get())
  //                     ->GetValue() -
  //                 dynamic_cast<FloatPin*>(input_node->GetInputPins()[1].get())
  //                     ->GetValue());
  //         dynamic_cast<FloatPin*>(input_node->GetOutputPins()[2].get())
  //             ->SetValue(
  //                 dynamic_cast<FloatPin*>(input_node->GetOutputPins()[2].get())
  //                     ->GetValue() -
  //                 dynamic_cast<FloatPin*>(input_node->GetInputPins()[2].get())
  //                     ->GetValue());
  //       }
  //     }

  //     for (auto& input_node_output_pin : input_node->GetOutputPins()) {
  //       if (auto* float_input_node_output_pin =
  //               dynamic_cast<FloatPin*>(input_node_output_pin.get())) {
  //         for (auto& link : links_) {
  //           if (link.start_pin_id == input_node_output_pin->GetId()) {
  //             auto* end_pin = FindPin(link.end_pin_id);

  //             if ((end_pin == nullptr) || (end_pin->ui_data_.node ==
  //             nullptr)) {
  //               continue;
  //             }

  //             if (auto* float_end_pin = dynamic_cast<FloatPin*>(end_pin)) {
  //               float_end_pin->SetValue(
  //                   float_end_pin->GetValue() +
  //                   float_input_node_output_pin->GetValue());

  //               for (auto& output_pin :
  //                    end_pin->ui_data_.node->GetOutputPins()) {
  //                 if (auto* float_output_pin =
  //                         dynamic_cast<FloatPin*>(output_pin.get())) {
  //                   float_output_pin->SetValue(
  //                       float_output_pin->GetValue() +
  //                       float_input_node_output_pin->GetValue());
  //                 }

  //                 next_input_nodes.emplace(end_pin->ui_data_.node);
  //               }
  //             }
  //           }
  //         }
  //       }
  //     }
  //   }

  //   input_nodes.assign(next_input_nodes.begin(), next_input_nodes.end());
  // }
}
}  // namespace esc::core