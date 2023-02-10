#include "core_client_node.h"

#include <memory>

#include "core_float_pin.h"
#include "core_flow_pin.h"
#include "core_node.h"
#include "esc_id_generator.h"

namespace esc {
namespace {
class Node;
class NodeFactory;

auto CreateNodeWriter(std::shared_ptr<Node> node)
    -> std::unique_ptr<INodeWriter>;
auto CreateNodeDrawer(std::shared_ptr<Node> node)
    -> std::unique_ptr<INodeDrawer>;
auto CreateNodeFactoryWriter(std::shared_ptr<NodeFactory> node_factory)
    -> std::unique_ptr<INodeFactoryWriter>;
auto CreateNodeFactoryDrawer(std::shared_ptr<NodeFactory> node_factory)
    -> std::unique_ptr<INodeFactoryDrawer>;

// NOLINTNEXTLINE(*-multiple-inheritance)
class Node : public INode, public std::enable_shared_from_this<Node> {
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

  auto CreateWriter() -> std::unique_ptr<INodeWriter> override {
    return CreateNodeWriter(shared_from_this());
  }

  auto CreateDrawer() -> std::unique_ptr<INodeDrawer> override {
    return CreateNodeDrawer(shared_from_this());
  }
};

class NodeParser : public INodeParser {
 public:
  auto ParseFromJson(const crude_json::value& json) const
      -> std::shared_ptr<INode> override {
    IdGenerator TEMP_GENERATOR{};
    return std::make_shared<Node>(TEMP_GENERATOR);
  }
};

class NodeWriter : public INodeWriter {
 public:
  explicit NodeWriter(std::shared_ptr<Node> node) : node_{std::move(node)} {}

  auto GetTypeName() const -> std::string override { return "ClientNode"; }

  auto WriteToJson() const -> crude_json::value override { return {}; }

 private:
  std::shared_ptr<Node> node_{};
};

auto CreateNodeWriter(std::shared_ptr<Node> node)
    -> std::unique_ptr<INodeWriter> {
  return std::make_unique<NodeWriter>(std::move(node));
}

class NodeDrawer : public INodeDrawer {
 public:
  explicit NodeDrawer(std::shared_ptr<Node> node) : node_{std::move(node)} {}

  auto GetLabel() const -> std::string override {
    return CreateClientNodeFactory()->CreateDrawer()->GetLabel();
  }

  auto GetColor() const -> ImColor override {
    return CreateClientNodeFactory()->CreateDrawer()->GetColor();
  }

 private:
  std::shared_ptr<Node> node_{};
};

auto CreateNodeDrawer(std::shared_ptr<Node> node)
    -> std::unique_ptr<INodeDrawer> {
  return std::make_unique<NodeDrawer>(std::move(node));
}

// NOLINTNEXTLINE(*-multiple-inheritance)
class NodeFactory : public INodeFactory,
                    public std::enable_shared_from_this<NodeFactory> {
 public:
  auto CreateNode(IdGenerator& id_generator)
      -> std::shared_ptr<INode> override {
    return std::make_shared<Node>(id_generator);
  }

  auto CreateNodeParser() -> std::unique_ptr<INodeParser> override {
    return std::make_unique<NodeParser>();
  }

  auto CreateWriter() -> std::unique_ptr<INodeFactoryWriter> override {
    return CreateNodeFactoryWriter(shared_from_this());
  }

  auto CreateDrawer() -> std::unique_ptr<INodeFactoryDrawer> override {
    return CreateNodeFactoryDrawer(shared_from_this());
  }
};

class NodeFactoryParser : public INodeFactoryParser {
 public:
  auto GetTypeName() const -> std::string override { return "ClientNode"; }

  auto ParseFromJson(const crude_json::value& json) const
      -> std::shared_ptr<INodeFactory> override {
    return std::make_shared<NodeFactory>();
  }
};

class NodeFactoryWriter : public INodeFactoryWriter {
 public:
  explicit NodeFactoryWriter(std::shared_ptr<NodeFactory> node_factory)
      : node_factory_{std::move(node_factory)} {}

  auto GetTypeName() const -> std::string override { return "ClientNode"; }

  auto WriteToJson() const -> crude_json::value override { return {}; }

 private:
  std::shared_ptr<NodeFactory> node_factory_{};
};

auto CreateNodeFactoryWriter(std::shared_ptr<NodeFactory> node_factory)
    -> std::unique_ptr<INodeFactoryWriter> {
  return std::make_unique<NodeFactoryWriter>(std::move(node_factory));
}

class NodeFactoryDrawer : public INodeFactoryDrawer {
 public:
  explicit NodeFactoryDrawer(std::shared_ptr<NodeFactory> node_factory)
      : node_factory_{std::move(node_factory)} {}

  auto GetLabel() const -> std::string override { return "Client"; }

  auto GetColor() const -> ImColor override { return {0, 255, 0}; }

 private:
  std::shared_ptr<NodeFactory> node_factory_{};
};

auto CreateNodeFactoryDrawer(std::shared_ptr<NodeFactory> node_factory)
    -> std::unique_ptr<INodeFactoryDrawer> {
  return std::make_unique<NodeFactoryDrawer>(std::move(node_factory));
}
}  // namespace

auto CreateClientNodeFactory() -> std::shared_ptr<INodeFactory> {
  return std::make_unique<NodeFactory>();
}

auto CreateClientNodeFactoryParser() -> std::unique_ptr<INodeFactoryParser> {
  return std::make_unique<NodeFactoryParser>();
}
}  // namespace esc