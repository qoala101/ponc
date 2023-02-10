#include "core_coupler_node.h"

#include "core_empty_pin.h"
#include "core_float_pin.h"
#include "core_flow_pin.h"
#include "esc_cpp.h"

namespace esc {
namespace {
class Node123;
class NodeFactory;

auto CreateNodeDrawer(std::shared_ptr<Node123> node)
    -> std::unique_ptr<INodeDrawer>;
auto CreateNodeFactoryDrawer(std::shared_ptr<NodeFactory> node_factory)
    -> std::unique_ptr<INodeFactoryDrawer>;

// NOLINTNEXTLINE(*-multiple-inheritance)
class Node123 : public INode, public std::enable_shared_from_this<Node123> {
 public:
  explicit Node123(esc::IdGenerator& id_generator)
      : INode{id_generator.GetNext<ne::NodeId>(),
             {std::make_shared<FlowPin>(id_generator.GetNext<ne::PinId>(),
                                        PinKind::Input, false),
              std::make_shared<FloatPin>(id_generator.GetNext<ne::PinId>(), "",
                                         PinKind::Input, false),
              std::make_shared<FloatPin>(id_generator.GetNext<ne::PinId>(), "",
                                         PinKind::Input, false)},
             {std::make_shared<EmptyPin>(id_generator.GetNext<ne::PinId>(),
                                         PinKind::Output),
              std::make_shared<FlowPin>(id_generator.GetNext<ne::PinId>(),
                                        PinKind::Output, false),
              std::make_shared<FlowPin>(id_generator.GetNext<ne::PinId>(),
                                        PinKind::Output, false)}} {}

  auto CreateDrawer() -> std::unique_ptr<INodeDrawer> override {
    return CreateNodeDrawer(shared_from_this());
  }

  auto GetCouplerPercentageIndex() -> int& { return coupler_percentage_index_; }

  void SetCouplerPercentageIndex(int index) {
    coupler_percentage_index_ = index;
  }

 private:
  int coupler_percentage_index_{};
};

class NodeDrawer : public INodeDrawer {
 public:
  explicit NodeDrawer(std::shared_ptr<Node123> node) : node_{std::move(node)} {}

  auto GetLabel() const -> std::string override {
    return CreateCouplerNodeFactory()->CreateDrawer()->GetLabel();
  }

  auto GetColor() const -> ImColor override {
    return CreateCouplerNodeFactory()->CreateDrawer()->GetColor();
  }

 private:
  std::shared_ptr<Node123> node_{};
};

auto CreateNodeDrawer(std::shared_ptr<Node123> node)
    -> std::unique_ptr<INodeDrawer> {
  return std::make_unique<NodeDrawer>(std::move(node));
}

// NOLINTNEXTLINE(*-multiple-inheritance)
class NodeFactory : public INodeFactory,
                    public std::enable_shared_from_this<NodeFactory> {
 public:
  auto CreateNode(IdGenerator& id_generator) -> std::shared_ptr<INode> override {
    return std::make_shared<Node123>(id_generator);
  }

  auto CreateDrawer() -> std::unique_ptr<INodeFactoryDrawer> override {
    return CreateNodeFactoryDrawer(shared_from_this());
  }
};

class NodeFactoryDrawer : public INodeFactoryDrawer {
 public:
  explicit NodeFactoryDrawer(std::shared_ptr<NodeFactory> node_factory)
      : node_factory_{std::move(node_factory)} {}

  auto GetLabel() const -> std::string override { return "Coupler 1x2"; }

  auto GetColor() const -> ImColor override { return {255, 0, 255}; }

 private:
  std::shared_ptr<NodeFactory> node_factory_{};
};

auto CreateNodeFactoryDrawer(std::shared_ptr<NodeFactory> node_factory)
    -> std::unique_ptr<INodeFactoryDrawer> {
  return std::make_unique<NodeFactoryDrawer>(std::move(node_factory));
}
}  // namespace

auto CreateCouplerNodeFactory() -> std::shared_ptr<INodeFactory> {
  return std::make_unique<NodeFactory>();
}
}  // namespace esc