#include "esc_app_state.h"

#include <memory>
#include <utility>

#include "core_app.h"
#include "core_diagram.h"
#include "core_id_generator.h"
#include "crude_json.h"
#include "impl_client_node.h"
#include "impl_input_node.h"
#include "impl_splitter_node.h"
#include "json_diagram_serializer.h"

namespace esc {
namespace {
auto CreateNodeFactories() {
  auto node_factories = std::vector<std::shared_ptr<core::INodeFactory>>{
      impl::InputNode::CreateNodeFactory(),
      impl::ClientNode::CreateNodeFactory()
      // ,
      // esc::CreateCouplerNodeFactory()
  };

  for (auto num_outputs : {2, 4, 8, 16}) {
    node_factories.emplace_back(
        impl::SplitterNode::CreateNodeFactory(num_outputs));
  }

  return node_factories;
}

auto CreateNodeFactoryParsers() {
  auto factory_parsers =
      std::vector<std::unique_ptr<json::INodeFactoryParser>>{};
  factory_parsers.emplace_back(impl::InputNode::CreateNodeFactoryParser());
  factory_parsers.emplace_back(impl::ClientNode::CreateNodeFactoryParser());
  factory_parsers.emplace_back(impl::SplitterNode::CreateNodeFactoryParser());
  return factory_parsers;
}

auto FindMaxId(const core::Diagram &diagram) {
  auto max_id = uintptr_t{};

  for (const auto &node_factory : diagram.GetNodeFactories()) {
    for (const auto &node : node_factory->GetNodes()) {
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

AppState::AppState() { ResetDiagram(); }

void AppState::ResetDiagram() {
  app_.SetDiagram(core::Diagram{CreateNodeFactories()});
  id_generator_.emplace();
}

void AppState::OpenDiagramFromFile(const std::string &file_path) {
  const auto json = crude_json::value::load(file_path).first;

  auto diagram =
      json::DiagramSerializer::ParseFromJson(json, CreateNodeFactoryParsers());
  auto max_id = FindMaxId(diagram);

  app_.SetDiagram(std::move(diagram));
  id_generator_.emplace(max_id);
}

void AppState::SaveDiagramToFile(const std::string &file_path) const {
  const auto *diagram = app_.GetDiagram();
  const auto json = json::DiagramSerializer::WriteToJson(*diagram);
  json.save(file_path);
}
}  // namespace esc