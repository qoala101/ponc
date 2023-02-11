#include "impl_client_node.h"

#include <memory>

#include "core_float_pin.h"
#include "core_flow_pin.h"
#include "core_i_node.h"
#include "crude_json.h"
#include "draw_i_node_drawer.h"
#include "draw_i_node_factory_drawer.h"
#include "esc_id_generator.h"
#include "json_node_serializer.h"

namespace esc::impl {
namespace {
class Node;
class NodeFactory;

constexpr auto kTypeName = "ClientNode";

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
  explicit Node(esc::IdGenerator& id_generator)
      : INode{id_generator.GetNext<ne::NodeId>(),
              {std::make_shared<FlowPin>(id_generator.GetNext<ne::PinId>(),
                                         PinKind::Input, false),
               std::make_shared<FloatPin>(id_generator.GetNext<ne::PinId>(),
                                          "min", PinKind::Input, true),
               std::make_shared<FloatPin>(id_generator.GetNext<ne::PinId>(),
                                          "max", PinKind::Input, true)},
              {}} {}

  auto CreateWriter() -> std::unique_ptr<json::INodeWriter> override {
    return CreateNodeWriter(shared_from_this());
  }

  auto CreateDrawer() -> std::unique_ptr<draw::INodeDrawer> override {
    return CreateNodeDrawer(shared_from_this());
  }
};

class NodeParser : public json::INodeParser {
 private:
  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto ParseFromJson(const crude_json::value& json) const
      -> std::shared_ptr<core::INode> override {
    IdGenerator TEMP_GENERATOR{};
    return std::make_shared<Node>(TEMP_GENERATOR);
  }
};

class NodeWriter : public json::INodeWriter {
 public:
  explicit NodeWriter(std::shared_ptr<Node> node) : node_{std::move(node)} {}

 private:
  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto WriteToJson() const -> crude_json::value override {
    auto json = crude_json::value{};
    json["min"] = static_cast<crude_json::number>(10);
    json["max"] = static_cast<crude_json::number>(20);
    return json;
  }

 private:
  std::shared_ptr<Node> node_{};
};

auto CreateNodeWriter(std::shared_ptr<Node> node)
    -> std::unique_ptr<json::INodeWriter> {
  return std::make_unique<NodeWriter>(std::move(node));
}

class NodeDrawer : public draw::INodeDrawer {
 public:
  explicit NodeDrawer(std::shared_ptr<Node> node) : node_{std::move(node)} {}

  auto GetLabel() const -> std::string override {
    return ClientNode::CreateNodeFactory()->CreateDrawer()->GetLabel();
  }

  auto GetColor() const -> ImColor override {
    return ClientNode::CreateNodeFactory()->CreateDrawer()->GetColor();
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
  auto CreateNode(IdGenerator& id_generator)
      -> std::shared_ptr<core::INode> override {
    return std::make_shared<Node>(id_generator);
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
};

class NodeFactoryParser : public json::INodeFactoryParser {
 public:
  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto ParseFromJson(const crude_json::value& json) const
      -> std::shared_ptr<core::INodeFactory> override {
    return std::make_shared<NodeFactory>();
  }
};

class NodeFactoryWriter : public json::INodeFactoryWriter {
 public:
  explicit NodeFactoryWriter(std::shared_ptr<NodeFactory> node_factory)
      : node_factory_{std::move(node_factory)} {}

  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto WriteToJson() const -> crude_json::value override { return {}; }

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

  auto GetLabel() const -> std::string override { return "Client"; }

  auto GetColor() const -> ImColor override { return {0, 255, 0}; }

 private:
  std::shared_ptr<NodeFactory> node_factory_{};
};

auto CreateNodeFactoryDrawer(std::shared_ptr<NodeFactory> node_factory)
    -> std::unique_ptr<draw::INodeFactoryDrawer> {
  return std::make_unique<NodeFactoryDrawer>(std::move(node_factory));
}
}  // namespace

auto ClientNode::CreateNodeFactory() -> std::shared_ptr<core::INodeFactory> {
  return std::make_unique<NodeFactory>();
}

auto ClientNode::CreateNodeFactoryParser()
    -> std::unique_ptr<json::INodeFactoryParser> {
  return std::make_unique<NodeFactoryParser>();
}
}  // namespace esc::impl