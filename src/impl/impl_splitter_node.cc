#include "impl_splitter_node.h"

#include <memory>
#include <string>

#include "core_i_node.h"
#include "core_id_generator.h"
#include "crude_json.h"
#include "draw_flow_input_pin_drawer.h"
#include "draw_flow_output_pin_drawer.h"
#include "draw_i_node_drawer.h"
#include "draw_i_node_factory_drawer.h"
#include "draw_i_pin_drawer.h"
#include "imgui_node_editor.h"
#include "json_i_node_factory_writer.h"
#include "json_i_node_parser.h"
#include "json_i_node_writer.h"

namespace esc::impl {
namespace {
class Node;
class NodeFactory;

constexpr auto kTypeName = "SplitterNode";

auto CreateNodeWriter(std::shared_ptr<Node> node)
    -> std::unique_ptr<json::INodeWriter>;
auto CreateNodeDrawer(std::shared_ptr<Node> node)
    -> std::unique_ptr<draw::INodeDrawer>;
auto CreateNodeFactoryWriter(std::shared_ptr<NodeFactory> node_factory)
    -> std::unique_ptr<json::INodeFactoryWriter>;
auto CreateNodeFactoryDrawer(std::shared_ptr<NodeFactory> node_factory)
    -> std::unique_ptr<draw::INodeFactoryDrawer>;

// NOLINTNEXTLINE(*-multiple-inheritance)
class Node : public core::INode, public std::enable_shared_from_this<Node> {
 public:
  Node(ne::NodeId id, std::vector<ne::PinId> pin_ids)
      : INode{id, std::move(pin_ids)} {}

  auto CreateWriter() -> std::unique_ptr<json::INodeWriter> override {
    return CreateNodeWriter(shared_from_this());
  }

  auto CreateDrawer() -> std::unique_ptr<draw::INodeDrawer> override {
    return CreateNodeDrawer(shared_from_this());
  }

  auto GetNumOutputs() const {
    return static_cast<int>(GetPinIds().size()) - 2;
  }
};

class NodeParser : public json::INodeParser {
 private:
  auto ParseFromJson(ne::NodeId parsed_node_id,
                     std::vector<ne::PinId> parsed_pin_ids,
                     const crude_json::value& json) const
      -> std::shared_ptr<core::INode> override {
    return std::make_shared<Node>(parsed_node_id, std::move(parsed_pin_ids));
  }
};

class NodeWriter : public json::INodeWriter {
 public:
  explicit NodeWriter(std::shared_ptr<Node> node) : node_{std::move(node)} {}

 private:
  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto WriteToJson() const -> crude_json::value override { return {}; }

  std::shared_ptr<Node> node_{};
};

auto CreateNodeWriter(std::shared_ptr<Node> node)
    -> std::unique_ptr<json::INodeWriter> {
  return std::make_unique<NodeWriter>(std::move(node));
}

class DropPinDrawer : public draw::IPinDrawer {
 public:
  explicit DropPinDrawer(int num_outputs)
      : drop_{[num_outputs]() {
          switch (num_outputs) {
            case 2:
              return -4.3F;
            case 4:
              return -7.4F;
            case 8:
              return -10.7F;
            case 16:
            default:
              return -13.9F;
          };
        }()} {}

  auto GetLabel [[nodiscard]] () const -> std::string override {
    return "drop";
  }

  auto GetKind [[nodiscard]] () const -> ne::PinKind override {
    return ne::PinKind::Input;
  }

  auto GetFloat [[nodiscard]] () -> float* override { return &drop_; }

  auto IsEditable [[nodiscard]] () const -> bool override { return false; }

  auto IsConnectable [[nodiscard]] () const -> bool override { return false; }

 private:
  float drop_{};
};

class NodeDrawer : public draw::INodeDrawer {
 public:
  explicit NodeDrawer(std::shared_ptr<Node> node) : node_{std::move(node)} {}

  auto GetLabel() const -> std::string override {
    return SplitterNode::CreateNodeFactory(node_->GetNumOutputs())
        ->CreateDrawer()
        ->GetLabel();
  }

  auto GetColor() const -> ImColor override {
    return SplitterNode::CreateNodeFactory(node_->GetNumOutputs())
        ->CreateDrawer()
        ->GetColor();
  }

  auto CreatePinDrawer(ne::PinId pin_id) const
      -> std::unique_ptr<draw::IPinDrawer> override {
    const auto pin_index = node_->GetPinIndex(pin_id);

    if (pin_index == 0) {
      return std::make_unique<draw::FlowInputPinDrawer>();
    }

    if (pin_index == 1) {
      return std::make_unique<DropPinDrawer>(node_->GetNumOutputs());
    }

    return std::make_unique<draw::FlowOutputPinDrawer>();
  }

 private:
  std::shared_ptr<Node> node_{};
};

auto CreateNodeDrawer(std::shared_ptr<Node> node)
    -> std::unique_ptr<draw::INodeDrawer> {
  return std::make_unique<NodeDrawer>(std::move(node));
}

// NOLINTNEXTLINE(*-multiple-inheritance)
class NodeFactory : public core::INodeFactory,
                    public std::enable_shared_from_this<NodeFactory> {
 public:
  explicit NodeFactory(int num_outputs,
                       std::vector<std::shared_ptr<core::INode>> nodes = {})
      : INodeFactory{std::move(nodes)}, num_outputs_{num_outputs} {}

  auto CreateNode(core::IdGenerator& id_generator)
      -> std::shared_ptr<core::INode> override {
    return std::make_shared<Node>(
        id_generator.GetNext<ne::NodeId>(),
        [&id_generator, num_outputs = num_outputs_]() {
          auto pin_ids =
              std::vector<ne::PinId>{id_generator.GetNext<ne::PinId>(),
                                     id_generator.GetNext<ne::PinId>()};

          for (auto i = 0; i < num_outputs; ++i) {
            pin_ids.emplace_back(id_generator.GetNext<ne::PinId>());
          }

          return pin_ids;
        }());
  }

  auto CreateNodeParser() -> std::unique_ptr<json::INodeParser> override {
    return std::make_unique<NodeParser>();
  }

  auto CreateWriter() -> std::unique_ptr<json::INodeFactoryWriter> override {
    return CreateNodeFactoryWriter(shared_from_this());
  }

  auto CreateDrawer() -> std::unique_ptr<draw::INodeFactoryDrawer> override {
    return CreateNodeFactoryDrawer(shared_from_this());
  }

  auto GetNumOutputs() const { return num_outputs_; }

 private:
  int num_outputs_{};
};

class NodeFactoryParser : public json::INodeFactoryParser {
 public:
  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto ParseFromJson(std::vector<std::shared_ptr<core::INode>> parsed_nodes,
                     const crude_json::value& json) const
      -> std::shared_ptr<core::INodeFactory> override {
    const auto num_outputs =
        static_cast<int>(json["num_outputs"].get<crude_json::number>());
    return std::make_shared<NodeFactory>(num_outputs, std::move(parsed_nodes));
  }
};

class NodeFactoryWriter : public json::INodeFactoryWriter {
 public:
  explicit NodeFactoryWriter(std::shared_ptr<NodeFactory> node_factory)
      : node_factory_{std::move(node_factory)} {}

  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto WriteToJson() const -> crude_json::value override {
    auto json = crude_json::value{};
    json["num_outputs"] =
        static_cast<crude_json::number>(node_factory_->GetNumOutputs());
    return json;
  }

 private:
  std::shared_ptr<NodeFactory> node_factory_{};
};

auto CreateNodeFactoryWriter(std::shared_ptr<NodeFactory> node_factory)
    -> std::unique_ptr<json::INodeFactoryWriter> {
  return std::make_unique<NodeFactoryWriter>(std::move(node_factory));
}

class NodeFactoryDrawer : public draw::INodeFactoryDrawer {
 public:
  explicit NodeFactoryDrawer(std::shared_ptr<NodeFactory> node_factory)
      : node_factory_{std::move(node_factory)} {}

  auto GetLabel() const -> std::string override {
    return "Splitter 1x" + std::to_string(node_factory_->GetNumOutputs());
  }

  auto GetColor() const -> ImColor override {
    return {0, 0, 127 + 128 / node_factory_->GetNumOutputs()};
  }

 private:
  std::shared_ptr<NodeFactory> node_factory_{};
};

auto CreateNodeFactoryDrawer(std::shared_ptr<NodeFactory> node_factory)
    -> std::unique_ptr<draw::INodeFactoryDrawer> {
  return std::make_unique<NodeFactoryDrawer>(std::move(node_factory));
}
}  // namespace

auto SplitterNode::CreateNodeFactory(int num_outputs)
    -> std::shared_ptr<core::INodeFactory> {
  return std::make_unique<NodeFactory>(num_outputs);
}

auto SplitterNode::CreateNodeFactoryParser()
    -> std::unique_ptr<json::INodeFactoryParser> {
  return std::make_unique<NodeFactoryParser>();
}
}  // namespace esc::impl