#include "core_diagram.h"

#include <crude_json.h>
#include <imgui_node_editor.h>
#include <imgui_node_editor_internal.h>
#include <sys/types.h>

#include <cstring>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "core_link.h"
#include "cpp_assert.h"
#include "draw_i_node_drawer.h"
#include "esc_app.h"
#include "esc_types.h"
#include "imgui.h"

namespace esc::core {
Diagram::Diagram(std::vector<std::shared_ptr<IFamily>> families,
                 std::vector<Link> links)
    : families_{std::move(families)}, links_{std::move(links)} {}

Diagram::~Diagram() {
  for (const auto& link : links_) {
    ne::DeleteLink(link.id);
  }

  for (const auto& family : families_) {
    for (const auto& node : family->GetNodes()) {
      ne::DeleteNode(node->GetId());
    }
  }
}

auto Diagram::GetLinks() const -> const std::vector<Link>& { return links_; }

auto Diagram::FindNode(ne::NodeId id) -> INode& {
  for (const auto& family : families_) {
    for (const auto& node : family->GetNodes()) {
      if (node->GetId() == id) {
        return *node;
      }
    }
  }

  cpp::Expects(false);
}

auto Diagram::FindPin(ne::PinId id) -> std::unique_ptr<draw::IPinDrawer> {
  for (const auto& family : families_) {
    for (const auto& node : family->GetNodes()) {
      for (const auto pin_id : node->GetPinIds()) {
        if (pin_id == id) {
          return node->CreateDrawer()->CreatePinDrawer(pin_id);
        }
      }
    }
  }

  cpp::Expects(false);
}

auto Diagram::FindLink(ne::LinkId id) -> Link& {
  for (auto& link : links_) {
    if (link.id == id) {
      return link;
    }
  }

  cpp::Expects(false);
}

auto Diagram::EmplaceLink(const Link& link) -> Link& {
  return links_.emplace_back(link);
}

void Diagram::EraseLink(ne::LinkId linkId) {
  auto id = std::find_if(links_.begin(), links_.end(),
                         [linkId](auto& link) { return link.id == linkId; });
  if (id != links_.end()) links_.erase(id);
}

void Diagram::EraseNode(ne::NodeId id) {
  for (const auto& family : families_) {
    for (const auto& node : family->GetNodes()) {
      if (node->GetId() == id) {
        return family->EraseNode(id);
      }
    }
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

auto Diagram::GetFamilies() const
    -> const std::vector<std::shared_ptr<IFamily>>& {
  return families_;
}

void Diagram::OnFrame() { UpdatePinValues(); }

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