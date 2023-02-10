#include "json_diagram_serializer.h"

#include <memory>

#include "core_diagram.h"
#include "core_node.h"
#include "crude_json.h"

namespace esc::json {
namespace {
auto ParseNodeFactories(const crude_json::value& json,
                        const std::vector<std::unique_ptr<INodeFactoryParser>>&
                            node_factory_parsers) {
  const auto node_factories_size =
      json["node_factories_size"].get<crude_json::number>();
  const auto& node_factories_json = json["node_factories"];

  auto parsed_node_factories = std::vector<std::shared_ptr<INodeFactory>>{};

  for (auto i = 0; i < node_factories_size; ++i) {
    const auto& node_factory_json = node_factories_json[i];
    const auto& parsed_type_name =
        node_factory_json["type"].get<crude_json::string>();

    for (const auto& parser : node_factory_parsers) {
      if (parser->GetTypeName() == parsed_type_name) {
        parsed_node_factories.emplace_back(
            parser->ParseFromJson(node_factory_json["data"]));
      }
    }
  }

  return parsed_node_factories;
}

auto ParseNodes(
    const crude_json::value& json,
    const std::vector<std::shared_ptr<INodeFactory>>& parsed_node_factories) {
  const auto nodes_size = json["nodes_size"].get<crude_json::number>();
  const auto& nodes_json = json["nodes"];

  auto parsed_nodes = std::vector<std::shared_ptr<INode>>{};

  for (auto i = 0; i < nodes_size; ++i) {
    const auto& node_json = nodes_json[i];
    const auto& parsed_type_name = node_json["type"].get<crude_json::string>();

    for (const auto& node_factory : parsed_node_factories) {
      if (node_factory->CreateWriter()->GetTypeName() == parsed_type_name) {
        parsed_nodes.emplace_back(
            node_factory->CreateNodeParser()->ParseFromJson(node_json["data"]));
      }
    }
  }

  return parsed_nodes;
}

void WriteNodeFactories(
    const std::vector<std::shared_ptr<INodeFactory>>& node_factories,
    crude_json::value& json) {
  json["node_factories_size"] =
      static_cast<crude_json::number>(node_factories.size());
  auto& node_factories_json = json["node_factories"];

  for (auto i = 0; i < static_cast<int>(node_factories.size()); ++i) {
    auto& node_factory_json = node_factories_json[i];

    const auto& node_factory = node_factories[i];
    const auto writer = node_factory->CreateWriter();

    node_factory_json["type"] = writer->GetTypeName();
    node_factory_json["data"] = writer->WriteToJson();
  }
}

void WriteNodes(const std::vector<std::shared_ptr<INode>>& nodes,
                crude_json::value& json) {
  json["nodes_size"] = static_cast<crude_json::number>(nodes.size());
  auto& nodes_json = json["nodes"];

  for (auto i = 0; i < static_cast<int>(nodes.size()); ++i) {
    auto& node_json = nodes_json[i];

    const auto& node = nodes[i];
    const auto writer = node->CreateWriter();

    node_json["type"] = writer->GetTypeName();
    node_json["data"] = writer->WriteToJson();
  }
}
}  // namespace

auto DiagramSerializer::ParseFromJson(
    const crude_json::value& json,
    const std::vector<std::unique_ptr<INodeFactoryParser>>&
        node_factory_parsers) -> core::Diagram {
  auto parsed_node_factories = ParseNodeFactories(json, node_factory_parsers);
  auto parsed_nodes = ParseNodes(json, parsed_node_factories);
  return core::Diagram{
      std::move(parsed_node_factories), std::move(parsed_nodes), {}};

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
  //            static_cast<uint64_t>(
  //                link_json["start_pin_id"].get<crude_json::number>()),
  //            static_cast<uint64_t>(
  //                link_json["end_pin_id"].get<crude_json::number>())});
  // }
}

auto DiagramSerializer::WriteToJson(const core::Diagram& diagram)
    -> crude_json::value {
  auto json = crude_json::value{};
  WriteNodeFactories(diagram.GetNodeFactories(), json);
  WriteNodes(diagram.GetNodes(), json);
  return json;

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
}  // namespace esc::json