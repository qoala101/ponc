#include "core_splitter_node.h"
#include <memory>

#include "core_float_pin.h"
#include "core_flow_pin.h"
#include "core_node.h"
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
  explicit Node123(esc::IdGenerator& id_generator, int num_outputs)
      : INode{
            id_generator.GetNext<ne::NodeId>(),
            {std::make_shared<FlowPin>(id_generator.GetNext<ne::PinId>(),
                                       PinKind::Input, false),
             std::make_shared<FloatPin>(id_generator.GetNext<ne::PinId>(), "",
                                        PinKind::Input, false)},
            [&id_generator, num_outputs]() {
              auto pins = std::vector<std::shared_ptr<Pin>>{};

              for (auto i = 0; i < num_outputs; ++i) {
                pins.emplace_back(std::make_shared<FlowPin>(
                    id_generator.GetNext<ne::PinId>(), PinKind::Output, false));
              }

              return pins;
            }()} {}

  auto CreateDrawer() -> std::unique_ptr<INodeDrawer> override {
    return CreateNodeDrawer(shared_from_this());
  }

  auto GetNumOutputs() const {
    return static_cast<int>(GetOutputPins().size());
  }
};

class NodeDrawer : public INodeDrawer {
 public:
  explicit NodeDrawer(std::shared_ptr<Node123> node) : node_{std::move(node)} {}

  auto GetLabel() const -> std::string override {
    return CreateSplitterNodeFactory(node_->GetNumOutputs())
        ->CreateDrawer()
        ->GetLabel();
  }

  auto GetColor() const -> ImColor override {
    return CreateSplitterNodeFactory(node_->GetNumOutputs())
        ->CreateDrawer()
        ->GetColor();
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
  explicit NodeFactory(int num_outputs) : num_outputs_{num_outputs} {}

  auto CreateNode(IdGenerator& id_generator) -> std::shared_ptr<INode> override {
    return std::make_shared<Node123>(id_generator, num_outputs_);
  }

  auto CreateDrawer() -> std::unique_ptr<INodeFactoryDrawer> override {
    return CreateNodeFactoryDrawer(shared_from_this());
  }

  auto GetNumOutputs() const { return num_outputs_; }

 private:
  int num_outputs_{};
};

class NodeFactoryDrawer : public INodeFactoryDrawer {
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
    -> std::unique_ptr<INodeFactoryDrawer> {
  return std::make_unique<NodeFactoryDrawer>(std::move(node_factory));
}
}  // namespace

auto CreateSplitterNodeFactory(int num_outputs) -> std::shared_ptr<INodeFactory> {
  return std::make_unique<NodeFactory>(num_outputs);
}
}  // namespace esc