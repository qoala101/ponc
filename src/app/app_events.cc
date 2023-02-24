#include "app_events.h"

#include "app_attenuator_node.h"
#include "app_input_node.h"
#include "app_state.h"
#include "core_i_node.h"
#include "core_project.h"
#include "draw_widgets.h"
#include "imgui_node_editor.h"
#include "json_project_serializer.h"

namespace esc {
// namespace {
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

auto FindMaxId(const core::Project &project) {
  auto max_id = uintptr_t{1};

  for (const auto &family : project.GetFamilies()) {
    max_id = std::max(family->GetId().Get(), max_id);
  }

  const auto &diagram = project.GetDiagram();

  for (const auto &node : diagram.GetNodes()) {
    max_id = std::max(node->GetId().Get(), max_id);

    for (const auto pin_id : core::GetAllPinIds(*node)) {
      max_id = std::max(pin_id.Get(), max_id);
    }
  }

  for (const auto &link : diagram.GetLinks()) {
    max_id = std::max(link.id.Get(), max_id);
  }

  return max_id;
}
// }  // namespace

void Events::OpenProjectFromFile::operator()(const AppState &app_state) const {
  // ResetDiagram{}(state);

  const auto json = crude_json::value::load(file_path).first;

  auto project =
      json::ProjectSerializer::ParseFromJson(json, CreateFamilyParsers());
  auto max_id = FindMaxId(project);

  app_state.project = std::move(project);
  app_state.id_generator = core::IdGenerator{max_id + 1};
}

void Events::SaveProjectToFile::operator()(const AppState &app_state) const {
  const auto json = json::ProjectSerializer::WriteToJson(app_state.project);
  json.save(file_path);
}

// void ResetDiagram::operator()(const AppState &app_state) const {
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
// }

void Events::EmplaceNode::operator()(const AppState &app_state) {
  const auto &new_node =
      app_state.project.GetDiagram().EmplaceNode(std::move(node));
  new_node->SetPosition(position);
}

void Events::CreateLink::operator()(const AppState &app_state) const {
  app_state.project.GetDiagram().EmplaceLink(
      {.id = app_state.id_generator.GetNext<ne::LinkId>(),
       .start_pin_id = start_pin_id,
       .end_pin_id = end_pin_id});
}

void Events::DeleteLink::operator()(const AppState &app_state) const {
  ne::DeleteLink(link_id);
  app_state.project.GetDiagram().EraseLink(link_id);
}

// void DeleteNode::operator()(const AppState &app_state) const {
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

// void DeleteNodeWithLinks::operator()(const AppState &app_state) const {
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

// void DeleteLink::operator()(const AppState &app_state) const {
//   ne::DeleteLink(link_id);
//   state.core_state->diagram_.EraseLink(link_id);
// }

// void CreateGroup::operator()(const AppState &app_state) const {
//   // auto nodes = std::vector<std::shared_ptr<INode>>{};

//   // for (const auto node_id : node_ids) {
//   //   nodes.emplace_back(FindNodePTR(node_id));
//   // }

//   // return groups_.emplace_back(std::move(nodes));

//   // group.name_ = "TEMP_NAME";

//   // auto &group = state.core_state->diagram_.EmplaceGroup(node_ids);
// }
}  // namespace esc