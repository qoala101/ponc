#include "esc_app_state.h"

#include <memory>

#include "core_app.h"
#include "crude_json.h"
#include "esc_id_generator.h"
#include "impl_client_node.h"
#include "json_diagram_serializer.h"

namespace esc {
namespace {
auto CreateNodeFactoryParsers() {
  auto factory_parsers =
      std::vector<std::unique_ptr<json::INodeFactoryParser>>{};
  factory_parsers.emplace_back(
      impl::ClientNode::CreateNodeFactoryParser());
  return factory_parsers;
}
}  // namespace

AppState::AppState()
    : app_{[]() {
        auto node_factories = std::vector<std::shared_ptr<core::INodeFactory>>{
            // esc::CreateInputNodeFactory(),
            impl::ClientNode::CreateNodeFactory()
            // ,
            // esc::CreateCouplerNodeFactory()
        };

        for (auto num_outputs : {2, 4, 8, 16}) {
          // node_factories.emplace_back(esc::CreateSplitterNodeFactory(num_outputs));
        }

        auto app = core::App{};
        app.SetDiagram(core::Diagram{std::move(node_factories), {}, {}});

        return app;
      }()},
      id_generator_{std::make_shared<IdGenerator>()} {}

void AppState::OpenDiagramFromFile(const std::string &file_path) {
  const auto json = crude_json::value::load(file_path).first;
  auto diagram =
      json::DiagramSerializer::ParseFromJson(json, CreateNodeFactoryParsers());
  app_.SetDiagram(std::move(diagram));
}

void AppState::SaveDiagramToFile(const std::string &file_path) const {
  const auto *diagram = app_.GetDiagram();
  const auto json = json::DiagramSerializer::WriteToJson(*diagram);
  json.save(file_path);
}
}  // namespace esc