#include "esc_state.h"

#include <memory>
#include <utility>

#include "core_app.h"
#include "core_diagram.h"
#include "core_id_generator.h"
#include "crude_json.h"
#include "impl_client_node.h"
#include "impl_coupler_node.h"
#include "impl_hub_node.h"
#include "impl_input_node.h"
#include "impl_splitter_node.h"
#include "json_diagram_serializer.h"

namespace esc {
namespace {
auto CreateFamilies() {
  auto families = std::vector<std::shared_ptr<core::IFamily>>{
      impl::HubNode::CreateFamily(), impl::InputNode::CreateFamily(),
      impl::ClientNode::CreateFamily(), impl::CouplerNode::CreateFamily()};

  for (auto num_outputs : {2, 4, 8, 16}) {
    families.emplace_back(impl::SplitterNode::CreateFamily(num_outputs));
  }

  return families;
}

auto CreateFamilyParsers() {
  auto family_parsers = std::vector<std::unique_ptr<json::IFamilyParser>>{};
  family_parsers.emplace_back(impl::HubNode::CreateFamilyParser());
  family_parsers.emplace_back(impl::InputNode::CreateFamilyParser());
  family_parsers.emplace_back(impl::ClientNode::CreateFamilyParser());
  family_parsers.emplace_back(impl::CouplerNode::CreateFamilyParser());
  family_parsers.emplace_back(impl::SplitterNode::CreateFamilyParser());
  return family_parsers;
}

auto FindMaxId(const core::Diagram &diagram) {
  auto max_id = uintptr_t{};

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

State::State() { ResetDiagram(); }

void State::OnFrame() const {
  for (const auto &family : app_.GetDiagram().GetFamilies()) {
    for (const auto &node : family->GetNodes()) {
      node->OnFrame(*this);
    }
  }
}

void State::ResetDiagram() {
  app_.SetDiagram(core::Diagram{CreateFamilies()});
  id_generator_.emplace();
}

void State::OpenDiagramFromFile(const std::string &file_path) {
  const auto json = crude_json::value::load(file_path).first;

  auto diagram =
      json::DiagramSerializer::ParseFromJson(json, CreateFamilyParsers());
  auto max_id = FindMaxId(diagram);

  app_.SetDiagram(std::move(diagram));
  id_generator_.emplace(max_id);
}

void State::SaveDiagramToFile(const std::string &file_path) const {
  const auto &diagram = app_.GetDiagram();
  const auto json = json::DiagramSerializer::WriteToJson(diagram);
  json.save(file_path);
}
}  // namespace esc