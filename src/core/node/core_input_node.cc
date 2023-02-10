#include "core_input_node.h"

#include "core_flow_pin.h"
#include "esc_cpp.h"

namespace esc {
namespace {
class Node;
class NodeFactory;

auto CreateNodeDrawer(std::shared_ptr<Node> node)
    -> std::unique_ptr<INodeDrawer>;
auto CreateNodeFactoryDrawer(std::shared_ptr<NodeFactory> node_factory)
    -> std::unique_ptr<INodeFactoryDrawer>;

// NOLINTNEXTLINE(*-multiple-inheritance)
class Node : public INode, public std::enable_shared_from_this<Node> {
 public:
  explicit Node(esc::IdGenerator& id_generator)
      : INode{id_generator.GetNext<ne::NodeId>(),
             {},
             {std::make_shared<FlowPin>(id_generator.GetNext<ne::PinId>(),
                                        PinKind::Output, true)}} {}

  auto CreateDrawer() -> std::unique_ptr<INodeDrawer> override {
    return CreateNodeDrawer(shared_from_this());
  }
};

class NodeDrawer : public INodeDrawer {
 public:
  explicit NodeDrawer(std::shared_ptr<Node> node) : node_{std::move(node)} {}

  auto GetLabel() const -> std::string override {
    return CreateInputNodeFactory()->CreateDrawer()->GetLabel();
  }

  auto GetColor() const -> ImColor override {
    return CreateInputNodeFactory()->CreateDrawer()->GetColor();
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
  auto CreateNode(IdGenerator& id_generator) -> std::shared_ptr<INode> override {
    return std::make_shared<Node>(id_generator);
  }

  auto CreateDrawer() -> std::unique_ptr<INodeFactoryDrawer> override {
    return CreateNodeFactoryDrawer(shared_from_this());
  }
};

class NodeFactoryDrawer : public INodeFactoryDrawer {
 public:
  explicit NodeFactoryDrawer(std::shared_ptr<NodeFactory> node_factory)
      : node_factory_{std::move(node_factory)} {}

  auto GetLabel() const -> std::string override { return "Input"; }

  auto GetColor() const -> ImColor override { return {255, 0, 0}; }

 private:
  std::shared_ptr<NodeFactory> node_factory_{};
};

auto CreateNodeFactoryDrawer(std::shared_ptr<NodeFactory> node_factory)
    -> std::unique_ptr<INodeFactoryDrawer> {
  return std::make_unique<NodeFactoryDrawer>(std::move(node_factory));
}
}  // namespace

auto CreateInputNodeFactory() -> std::shared_ptr<INodeFactory> {
  return std::make_unique<NodeFactory>();
}
}  // namespace esc