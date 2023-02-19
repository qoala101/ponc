// #include "app_events.h"

// #include "app_attenuator_node.h"
// #include "app_client_node.h"
// #include "app_coupler_node.h"
// #include "app_input_node.h"
// #include "app_splitter_node.h"
// #include "app_state.h"
// #include "core_i_node.h"
// #include "core_state.h"
// #include "draw_widgets.h"
// #include "json_diagram_serializer.h"

// namespace esc::event {
// namespace {
// auto CreateFamilies() {
//   auto families = std::vector<std::shared_ptr<core::IFamily>>{
//       InputNode::CreateFamily(), ClientNode::CreateFamily()};

//   for (auto percentage_index = 0; percentage_index < 10; ++percentage_index) {
//     families.emplace_back(CouplerNode::CreateFamily(percentage_index));
//   }

//   for (auto num_outputs : {2, 4, 8, 16}) {
//     families.emplace_back(SplitterNode::CreateFamily(num_outputs));
//   }

//   families.emplace_back(AttenuatorNode::CreateFamily());

//   return families;
// }

// auto CreateFamilyParsers() {
//   auto family_parsers = std::vector<std::unique_ptr<json::IFamilyParser>>{};
//   family_parsers.emplace_back(InputNode::CreateFamilyParser());
//   family_parsers.emplace_back(ClientNode::CreateFamilyParser());
//   family_parsers.emplace_back(CouplerNode::CreateFamilyParser());
//   family_parsers.emplace_back(SplitterNode::CreateFamilyParser());
//   family_parsers.emplace_back(AttenuatorNode::CreateFamilyParser());
//   family_parsers.emplace_back(core::PlaceholderFamily::CreateParser());
//   return family_parsers;
// }

// auto FindMaxId(const core::Diagram &diagram) {
//   auto max_id = uintptr_t{1};

//   for (const auto &family : diagram.GetFamilies()) {
//     for (const auto &node : family->GetNodes()) {
//       max_id = std::max(node->GetId().Get(), max_id);

//       for (const auto pin_id : core::GetAllPinIds(*node)) {
//         max_id = std::max(pin_id.Get(), max_id);
//       }
//     }
//   }

//   for (const auto &link : diagram.GetLinks()) {
//     max_id = std::max(link.id.Get(), max_id);
//   }

//   return max_id;
// }
// }  // namespace

// void OpenDiagramFromFile::operator()(StateNoQueue &state) const {
//   ResetDiagram{}(state);

//   const auto json = crude_json::value::load(file_path.data()).first;

//   auto diagram =
//       json::DiagramSerializer::ParseFromJson(json, CreateFamilyParsers());
//   auto max_id = FindMaxId(diagram);

//   state.core_state->diagram_ = std::move(diagram);
//   state.core_state->id_generator_ = core::IdGenerator{max_id + 1};
// }

// void SaveDiagramToFile::operator()(StateNoQueue &state) const {
//   const auto &diagram = state.core_state->diagram_;
//   const auto json = json::DiagramSerializer::WriteToJson(diagram);
//   json.save(file_path.data());
// }

// void ResetDiagram::operator()(StateNoQueue &state) const {
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

// void CreateNode::operator()(StateNoQueue &state) const {
//   auto family_lock = family.lock();

//   if (family_lock == nullptr) {
//     return;
//   }

//   const auto &new_node =
//       family_lock->EmplaceNode(state.core_state->id_generator_);
//   new_node->SetPosition(position);
// }

// void DeleteNode::operator()(StateNoQueue &state) const {
//   auto &diagram = state.core_state->diagram_;
//   const auto node = FindNode(diagram, node_id);
//   const auto node_flow = node.GetInitialFlow();
//   const auto node_position = node.GetPosition();

//   const auto &links = diagram.GetLinks();

//   auto connected_node_input_pin = std::optional<ne::PinId>{};
//   auto connected_node_output_pins = std::vector<ne::PinId>{};

//   if (node_flow.input_pin_flow.has_value()) {
//     if (diagram.FindLinkFromPin(node_flow.input_pin_flow->first) != nullptr) {
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

// void DeleteNodeWithLinks::operator()(StateNoQueue &state) const {
//   auto &diagram = state.core_state->diagram_;
//   const auto &links = diagram.GetLinks();
//   const auto &node = diagram.FindNode(node_id);
//   const auto node_pins = core::GetAllPinIds(node);

//   const auto links_to_erase = [&links, &node_pins]() {
//     auto links_to_erase = std::vector<ne::LinkId>{};

//     for (const auto &link : links) {
//       for (const auto node_pin : node_pins) {
//         if ((link.start_pin_id == node_pin) || (link.end_pin_id == node_pin)) {
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

// void DeleteLink::operator()(StateNoQueue &state) const {
//   ne::DeleteLink(link_id);
//   state.core_state->diagram_.EraseLink(link_id);
// }

// void CreateGroup::operator()(StateNoQueue &state) const {
//   // auto nodes = std::vector<std::shared_ptr<INode>>{};

//   // for (const auto node_id : node_ids) {
//   //   nodes.emplace_back(FindNodePTR(node_id));
//   // }

//   // return groups_.emplace_back(std::move(nodes));

//   // group.name_ = "TEMP_NAME";

//   // auto &group = state.core_state->diagram_.EmplaceGroup(node_ids);
// }
// }  // namespace esc::event