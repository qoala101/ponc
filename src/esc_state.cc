#include "esc_state.h"

#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "core_app.h"
#include "core_diagram.h"
#include "core_id_generator.h"
#include "core_placeholder_family.h"
#include "crude_json.h"
#include "imgui_node_editor.h"
#include "impl_attenuator_node.h"
#include "impl_client_node.h"
#include "impl_coupler_node.h"
#include "impl_input_node.h"
#include "impl_splitter_node.h"
#include "json_diagram_serializer.h"

namespace esc {
namespace {
auto CreateFamilies() {
  auto families = std::vector<std::shared_ptr<core::IFamily>>{
      impl::InputNode::CreateFamily(), impl::ClientNode::CreateFamily()};

  for (auto percentage_index = 0; percentage_index < 10; ++percentage_index) {
    families.emplace_back(impl::CouplerNode::CreateFamily(percentage_index));
  }

  for (auto num_outputs : {2, 4, 8, 16}) {
    families.emplace_back(impl::SplitterNode::CreateFamily(num_outputs));
  }

  families.emplace_back(impl::AttenuatorNode::CreateFamily());

  return families;
}

auto CreateFamilyParsers() {
  auto family_parsers = std::vector<std::unique_ptr<json::IFamilyParser>>{};
  family_parsers.emplace_back(impl::InputNode::CreateFamilyParser());
  family_parsers.emplace_back(impl::ClientNode::CreateFamilyParser());
  family_parsers.emplace_back(impl::CouplerNode::CreateFamilyParser());
  family_parsers.emplace_back(impl::SplitterNode::CreateFamilyParser());
  family_parsers.emplace_back(impl::AttenuatorNode::CreateFamilyParser());
  family_parsers.emplace_back(core::PlaceholderFamily::CreateParser());
  return family_parsers;
}

auto FindMaxId(const core::Diagram &diagram) {
  auto max_id = uintptr_t{1};

  for (const auto &family : diagram.GetFamilies()) {
    for (const auto &node : family->GetNodes()) {
      max_id = std::max(node->GetId().Get(), max_id);

      for (const auto pin_id : node->GetPinIds()) {
        max_id = std::max(pin_id.Get(), max_id);
      }
    }
  }

  for (const auto &link : diagram.GetLinks()) {
    max_id = std::max(link.id.Get(), max_id);
  }

  return max_id;
}
}  // namespace

State::State() { ResetDiagram(*this); }

void State::OpenDiagramFromFile(State &state, const std::string &file_path) {
  ResetDiagram(state);

  const auto json = crude_json::value::load(file_path).first;

  auto diagram =
      json::DiagramSerializer::ParseFromJson(json, CreateFamilyParsers());
  auto max_id = FindMaxId(diagram);

  state.app_.SetDiagram(std::move(diagram));
  state.id_generator_ = core::IdGenerator{max_id};
}

void State::SaveDiagramToFile(const State &state,
                              const std::string &file_path) {
  const auto &diagram = state.app_.GetDiagram();
  const auto json = json::DiagramSerializer::WriteToJson(diagram);
  json.save(file_path);
}

void State::ResetDiagram(State &state) {
  const auto &diagram = state.app_.GetDiagram();
  const auto &links = diagram.GetLinks();

  for (const auto &link : links) {
    ne::DeleteLink(link.id);
  }

  const auto &families = diagram.GetFamilies();

  for (const auto &family : families) {
    for (const auto &node : family->GetNodes()) {
      ne::DeleteNode(node->GetId());
    }
  }

  state.drawing_.not_yet_connected_pin_of_new_link_id.reset();
  state.drawing_.connect_new_node_to_existing_pin_id.reset();

  state.id_generator_ = core::IdGenerator{};
  state.app_.SetDiagram(core::Diagram{CreateFamilies()});
}

void State::EraseLink(State &state, ne::LinkId link_id) {
  ne::DeleteLink(link_id);
  state.app_.GetDiagram().EraseLink(link_id);
}

void State::EraseNodeAndConnectedLinks(State &state, ne::NodeId node_id) {
  auto &diagram = state.app_.GetDiagram();
  const auto &links = diagram.GetLinks();
  const auto &node = diagram.FindNode(node_id);
  const auto &node_pins = node.GetPinIds();

  const auto links_to_erase = [&links, &node_pins]() {
    auto links_to_erase = std::vector<ne::LinkId>{};

    for (const auto &link : links) {
      for (const auto node_pin : node_pins) {
        if ((link.start_pin_id == node_pin) || (link.end_pin_id == node_pin)) {
          links_to_erase.emplace_back(link.id);
        }
      }
    }
    return links_to_erase;
  }();

  for (const auto link : links_to_erase) {
    ne::DeleteLink(link);
  }

  for (const auto link : links_to_erase) {
    diagram.EraseLink(link);
  }

  ne::DeleteNode(node_id);
  state.app_.GetDiagram().EraseNode(node_id);
}

void State::ReplaceWithPlaceholder(State &state, ne::NodeId node_id) {
  auto &diagram = state.app_.GetDiagram();
  const auto &node = diagram.FindNode(node_id);
  const auto node_flow = node.GetInitialFlow();
  const auto node_position = node.GetPosition();

  EraseNodeAndConnectedLinks(state, node_id);

  auto &placeholder_family = diagram.GetPlaceholderFamily();
  auto &placeholder =
      placeholder_family.EmplaceNodeFromFlow(state.id_generator_, node_flow);

  placeholder.SetPosition(node_position);
}

void State::MakeGroupFromSelectedNodes(State &state, std::string group_name) {
  const auto selectedNodes = state.app_.GetDiagram().GetSelectedNodeIds();

  std::cout << selectedNodes.size() << " " << group_name << "\n";

  auto &group = state.app_.GetDiagram().EmplaceGroup(selectedNodes);
  group.SetName(std::move(group_name));
}

void State::OnFrame() {
  ExecuteEvents();
  flow_calculator_.OnFrame(*this);
}

void State::PostEvent(std::function<void(State &state)> event) {
  events_.emplace_back(std::move(event));
}

void State::ExecuteEvents() {
  for (const auto &event : events_) {
    event(*this);
  }

  events_.clear();
}
}  // namespace esc