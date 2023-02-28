#include "frame_node.h"

#include <functional>
#include <variant>
#include <vector>

#include "app_attenuator_node.h"
#include "app_input_node.h"
#include "core_diagram.h"
#include "core_link.h"
#include "coreui_i_node_traits.h"
#include "cpp_assert.h"
#include "cpp_scope.h"
#include "flow_tree.h"
#include "imgui.h"
#include "imgui_node_editor.h"
#include "json_i_family_parser.h"
#include "json_project_serializer.h"

namespace esc::coreui {
namespace {
auto GetPinKind(const coreui::IPinTraits& pin_drawer, const core::INode& node) {
  const auto pin_kind = pin_drawer.GetKind();

  if (pin_kind.has_value()) {
    return *pin_kind;
  }

  const auto pin_id = pin_drawer.GetPinId();
  Expects(pin_id.has_value());

  return core::GetPinKind(node, *pin_id);
}
}  // namespace

auto Frame::GetLinkAlpha(ne::LinkId link_id) const {
  if (creation.IsLinkBeingRepinned(link_id)) {
    return 1.F / 2;
  }

  return 1.F;
}

auto Frame::GetColorForFlowValue(float value) const {
  const auto& project = signal_get_project_();
  const auto& settings = project.GetSettings();

  if (!settings.color_flow) {
    return ImColor{1.F, 1.F, 1.F};
  }

  const auto blue = ImColor{0.F, 0.F, 1.F};
  const auto red = ImColor{1.F, 0.F, 0.F};

  if (value < settings.min_flow) {
    return blue;
  }

  if (value >= settings.max_flow) {
    return red;
  }

  const auto range = (settings.max_flow - settings.min_flow);
  const auto value_percentage = (value - settings.min_flow) / range;
  const auto low_percentage = (settings.low_flow - settings.min_flow) / range;
  const auto high_percentage = (settings.high - settings.min_flow) / range;

  auto percentage = 0.0F;
  auto start_color = ImColor{};
  auto end_color = ImColor{};

  const auto blue_green = ImColor{0.F, 1.F, 1.F};
  const auto green = ImColor{0.F, 1.F, 0.F};
  const auto green_red = ImColor{1.F, 1.F, 0.F};

  if (value_percentage < low_percentage) {
    percentage = value_percentage / low_percentage;
    start_color = blue;
    end_color = blue_green;
  } else if (value_percentage >= high_percentage) {
    percentage =
        (value_percentage - high_percentage) / (1.0F - high_percentage);
    start_color = green_red;
    end_color = red;
  } else {
    const auto low_high_range = (high_percentage - low_percentage);
    percentage = (value_percentage - low_percentage) / low_high_range;

    if (percentage < 0.5F) {
      percentage = percentage * 2;
      start_color = blue_green;
      end_color = green;
    } else {
      percentage = (percentage - 0.5F) * 2;
      start_color = green;
      end_color = green_red;
    }
  }

  return ImColor{start_color.Value.x +
                     percentage * (end_color.Value.x - start_color.Value.x),
                 start_color.Value.y +
                     percentage * (end_color.Value.y - start_color.Value.y),
                 start_color.Value.z +
                     percentage * (end_color.Value.z - start_color.Value.z)};
}

auto Frame::GetLinks() {
  const auto& project = signal_get_project_();
  const auto& diagram = project.GetDiagram();

  const auto flow_tree = flow::BuildFlowTree(diagram);
  const auto node_flows = flow::CalculateNodeFlows(flow_tree);

  auto links = std::vector<Link>{};

  for (const auto& link : diagram.GetLinks()) {
    const auto alpha = GetLinkAlpha(link.id);

    const auto start_pin_node = core::FindPinNode(diagram, link.start_pin_id);
    const auto node_flow = node_flows.at(start_pin_node->GetId().Get());
    const auto start_pin_flow = flow::GetPinFlow(node_flow, link.start_pin_id);
    auto color = GetColorForFlowValue(start_pin_flow);
    color.Value.w = alpha;

    auto& link_data = links.emplace_back();
    link_data.id = link.id;
    link_data.start_pin = link.start_pin_id;
    link_data.end_pin = link.end_pin_id;
    link_data.color = color;
    link_data.thickness = 2.F;
  }

  return links;
}

auto Frame::GetPinIconAlpha(ne::PinId pin_id) const {
  if (!creation.CanConnectToPin(pin_id)) {
    return 1.F / 4;
  }

  return 1.F;
}

auto Frame::GetCurve() -> std::optional<HandmadeLink> {
  if (!creation.IsRepinningLink()) {
    return std::nullopt;
  }

  const auto fixed_pin = creation.GetFixedPinOfLinkBeingRepinned();

  const auto& project = signal_get_project_();
  const auto& diagram = project.GetDiagram();
  auto curve = HandmadeLink{.color = ImColor{1.F, 1.F, 1.F}, .thickness = 4.F};

  // const auto fixed_pin_rect = nodes.pin_rects.at(fixed_pin.Get());
  // const auto fixed_pin_node = FindPinNode(diagram, fixed_pin);
  // const auto fixed_pin_kind = core::GetPinKind(*fixed_pin_node, fixed_pin);

  // if (fixed_pin_kind == ax::NodeEditor::PinKind::Input) {
  //   curve.end_pos = ImVec2{fixed_pin_rect.Min.x,
  //                          (fixed_pin_rect.Min.y + fixed_pin_rect.Max.y) /
  //                          2};
  // } else {
  //   curve.start_pos = ImVec2{fixed_pin_rect.Max.x,
  //                            (fixed_pin_rect.Min.y + fixed_pin_rect.Max.y) /
  //                            2};
  // }

  // if (creation.IsHoveringOverPin()) {
  //   if (creation.GetCanCreateLinkReason().first) {
  //     curve.color = ImColor{1.F / 2, 1.F, 1.F / 2};
  //   } else {
  //     curve.color = ImColor{1.F, 1.F / 2, 1.F / 2};
  //   }
  // }

  return curve;
}

// auto CreateFamilies() {
//   auto families = std::vector<std::shared_ptr<core::IFamily>>{
//       InputNode::CreateFamily(), ClientNode::CreateFamily()};

//   for (auto percentage_index = 0; percentage_index < 10; ++percentage_index)
//   {
//     families.emplace_back(CouplerNode::CreateFamily(percentage_index));
//   }

//   for (auto num_outputs : {2, 4, 8, 16}) {
//     families.emplace_back(SplitterNode::CreateFamily(num_outputs));
//   }

//   families.emplace_back(AttenuatorNode::CreateFamily());

//   return families;
// }

auto CreateFamilyParsers() {
  auto family_parsers = std::vector<std::unique_ptr<json::IFamilyParser>>{};
  family_parsers.emplace_back(InputNode::CreateFamilyParser());
  // family_parsers.emplace_back(ClientNode::CreateFamilyParser());
  // family_parsers.emplace_back(CouplerNode::CreateFamilyParser());
  // family_parsers.emplace_back(SplitterNode::CreateFamilyParser());
  family_parsers.emplace_back(AttenuatorNode::CreateFamilyParser());
  // family_parsers.emplace_back(core::PlaceholderFamily::CreateParser());
  return family_parsers;
}

Frame::Frame(SignalGetProject signal_get_project)
    : signal_get_project_{std::move(signal_get_project)},
      creation{[this]() { return signal_get_project_().GetDiagram(); },
               [this](ne::PinId start_pin_id, ne::PinId end_pin_id) {
                 CreateLink(start_pin_id, end_pin_id);
               },
               [this](ne::LinkId link_id) { DeleteLink(link_id); }} {}

void Frame::OnFrame() {
  for (const auto& event : events_) {
    event();
  }

  events_.clear();

  UpdateNodes();
  UpdateLinks();
}

void Frame::UpdateNodes() {
  const auto& project = signal_get_project_();
  const auto& diagram = project.GetDiagram();
  const auto flow_tree = flow::BuildFlowTree(diagram);
  const auto node_flows = flow::CalculateNodeFlows(flow_tree);
  const auto& settings = project.GetSettings();

  auto nodes = std::vector<Node>();

  for (const auto& node : diagram.GetNodes()) {
    auto& node_data = nodes.emplace_back();
    node_data.id = node->GetId();

    const auto traits = node->CreateUiTraits();
    const auto& node_flow = node_flows.at(node_data.id.Get());

    if (traits->HasHeader()) {
      node_data.header = NodeHeader{.label = traits->GetLabel()};

      if (settings.color_flow) {
        if (const auto input_flow = node_flow.input_pin_flow) {
          node_data.header->color = GetColorForFlowValue(input_flow->second);
        } else {
          node_data.header->color = ImColor{1.F, 1.F, 1.F};
        }
      } else {
        node_data.header->color = traits->GetColor();
      }
    }

    for (const auto& pin_drawer : traits->CreatePinTraits()) {
      const auto pin_kind = GetPinKind(*pin_drawer, *node);
      auto& pin_data = (pin_kind == ne::PinKind::Input)
                           ? node_data.input_pins.emplace_back()
                           : node_data.output_pins.emplace_back();

      pin_data.id = pin_drawer->GetPinId();
      pin_data.label = pin_drawer->GetLabel();

      if (const auto float_value = pin_drawer->GetFloat()) {
        pin_data.value = *float_value;
      }

      if (const auto not_flow_pin = !pin_data.id.has_value()) {
        continue;
      }

      const auto pin_flow = flow::GetPinFlow(node_flow, *pin_data.id);
      pin_data.color = GetColorForFlowValue(pin_flow);
      pin_data.color.Value.w = GetPinIconAlpha(*pin_data.id);
      pin_data.filled = FindPinLink(diagram, *pin_data.id).has_value();

      if (std::holds_alternative<std::monostate>(pin_data.value)) {
        pin_data.value = pin_flow;
      }
    }
  }
}

void Frame::UpdateLinks() {
  links = GetLinks();
  handmade_link = GetCurve();
}

auto Frame::GetProject() const -> core::Project& {
  return signal_get_project_();
}

void Frame::OpenProjectFromFile(std::string file_path) {
  events_.emplace_back([this, file_path = std::move(file_path)]() {
    const auto json = crude_json::value::load(file_path).first;
    signal_get_project_() =
        json::ProjectSerializer::ParseFromJson(json, CreateFamilyParsers());
  });
}

void Frame::SaveProjectToFile(std::string file_path) {
  events_.emplace_back([this, file_path = std::move(file_path)]() {
    const auto json =
        json::ProjectSerializer::WriteToJson(signal_get_project_());
    json.save(file_path);
  });
}

// void ResetDiagram::operator()(coreui::Frame &frame) const {
//   const auto &diagram = state.core_state->diagram_;
//   const auto &links = diagram.GetLinks();

//   for (const auto &link : links) {
//     ne::DeleteLink(link.id);
//   }

//   const auto &families = diagram.GetFamilies();

//   for (const auto &family : families) {
//     for (const auto &node : family->GetNodes()) {
//       ne::DeleteNode(node->GetId());
//     }
//   }

//   state.core_state->id_generator_ = core::IdGenerator{};
//   state.core_state->diagram_ = core::Diagram{CreateFamilies()};
// }z

void Frame::EmplaceNode(std::shared_ptr<core::INode> node,
                        const ImVec2& position) {
  events_.emplace_back([this, node = std::move(node), position]() mutable {
    const auto& new_node =
        signal_get_project_().GetDiagram().EmplaceNode(std::move(node));
    new_node->SetPosition(position);
  });
}

void Frame::CreateLink(ne::PinId start_pin_id, ne::PinId end_pin_id) {
  events_.emplace_back([this, start_pin_id, end_pin_id]() {
    auto& project = signal_get_project_();

    project.GetDiagram().EmplaceLink(
        {.id = project.GetIdGenerator().Generate<ne::LinkId>(),
         .start_pin_id = start_pin_id,
         .end_pin_id = end_pin_id});
  });
}

void Frame::DeleteLink(ne::LinkId link_id) {
  events_.emplace_back([this, link_id]() {
    ne::DeleteLink(link_id);
    signal_get_project_().GetDiagram().EraseLink(link_id);
  });
}

// void DeleteNode::operator()(coreui::Frame &frame) const {
//   auto &diagram = state.core_state->diagram_;
//   const auto node = FindNode(diagram, node_id);
//   const auto node_flow = node.GetInitialFlow();
//   const auto node_position = node.GetPosition();

//   const auto &links = diagram.GetLinks();

//   auto connected_node_input_pin = std::optional<ne::PinId>{};
//   auto connected_node_output_pins = std::vector<ne::PinId>{};

//   if (node_flow.input_pin_flow.has_value()) {
//     if (diagram.FindLinkFromPin(node_flow.input_pin_flow->first) != nullptr)
//     {
//       connected_node_input_pin = node_flow.input_pin_flow->first;
//     }
//   }

//   for (const auto &[pin, value] : node_flow.output_pin_flows) {
//     if (diagram.FindLinkFromPin(pin) != nullptr) {
//       connected_node_output_pins.emplace_back(pin);
//     }
//   }

//   ne::DeleteNode(node_id);
//   state.core_state->diagram_.EraseNode(node_id);

//   auto &free_pin_family = diagram.GetFreePinFamily();

//   if (connected_node_input_pin.has_value()) {
//     auto &free_pin = free_pin_family.EmplaceNodeFromFlow(
//         state.core_state->id_generator_, *connected_node_input_pin, true);

//     // free_pin.SetPosition(
//     //     state.draw_state->pin_poses_.at(connected_node_input_pin->Get()));
//   }

//   for (const auto pin : connected_node_output_pins) {
//     auto &free_pin = free_pin_family.EmplaceNodeFromFlow(
//         state.core_state->id_generator_, pin, false);
//     // free_pin.SetPosition(state.draw_state->pin_poses_.at(pin.Get()));
//   }
// }

// void DeleteNodeWithLinks::operator()(coreui::Frame &frame) const {
//   auto &diagram = state.core_state->diagram_;
//   const auto &links = diagram.GetLinks();
//   const auto &node = diagram.FindNode(node_id);
//   const auto node_pins = core::GetAllPinIds(node);

//   const auto links_to_erase = [&links, &node_pins]() {
//     auto links_to_erase = std::vector<ne::LinkId>{};

//     for (const auto &link : links) {
//       for (const auto node_pin : node_pins) {
//         if ((link.start_pin_id == node_pin) || (link.end_pin_id == node_pin))
//         {
//           links_to_erase.emplace_back(link.id);
//         }
//       }
//     }
//     return links_to_erase;
//   }();

//   for (const auto link : links_to_erase) {
//     ne::DeleteLink(link);
//   }

//   for (const auto link : links_to_erase) {
//     diagram.EraseLink(link);
//   }

//   ne::DeleteNode(node_id);
//   state.core_state->diagram_.EraseNode(node_id);
// }

// void DeleteLink::operator()(coreui::Frame &frame) const {
//   ne::DeleteLink(link_id);
//   state.core_state->diagram_.EraseLink(link_id);
// }

// void CreateGroup::operator()(coreui::Frame &frame) const {
//   // auto nodes = std::vector<std::shared_ptr<INode>>{};

//   // for (const auto node_id : node_ids) {
//   //   nodes.emplace_back(FindNodePTR(node_id));
//   // }

//   // return groups_.emplace_back(std::move(nodes));

//   // group.name_ = "TEMP_NAME";

//   // auto &group = state.core_state->diagram_.EmplaceGroup(node_ids);
// }
}  // namespace esc::coreui