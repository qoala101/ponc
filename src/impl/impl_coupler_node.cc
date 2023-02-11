#include "core_coupler_node.h"
#include "core_empty_pin.h"
#include "core_float_pin.h"
#include "core_flow_pin.h"
#include "esc_cpp.h"

namespace esc {
namespace {
class Node;
class Family;

auto CreateNodeDrawer(std::shared_ptr<Node> node)
    -> std::unique_ptr<INodeDrawer>;
auto CreateFamilyDrawer(std::shared_ptr<Family> family)
    -> std::unique_ptr<IFamilyDrawer>;

// NOLINTNEXTLINE(*-multiple-inheritance)
class Node : public INode, public std::enable_shared_from_this<Node> {
 public:
  explicit Node(esc::IdGenerator& id_generator)
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
  explicit NodeDrawer(std::shared_ptr<Node> node) : node_{std::move(node)} {}

  auto GetLabel() const -> std::string override {
    return CreateCouplerFamily()->CreateDrawer()->GetLabel();
  }

  auto GetColor() const -> ImColor override {
    return CreateCouplerFamily()->CreateDrawer()->GetColor();
  }

 private:
  std::shared_ptr<Node> node_{};
};

auto CreateNodeDrawer(std::shared_ptr<Node> node)
    -> std::unique_ptr<INodeDrawer> {
  return std::make_unique<NodeDrawer>(std::move(node));
}

// NOLINTNEXTLINE(*-multiple-inheritance)
class Family : public IFamily, public std::enable_shared_from_this<Family> {
 public:
  auto CreateNode(IdGenerator& id_generator)
      -> std::shared_ptr<INode> override {
    return std::make_shared<Node>(id_generator);
  }

  auto CreateDrawer() -> std::unique_ptr<IFamilyDrawer> override {
    return CreateFamilyDrawer(shared_from_this());
  }
};

class FamilyDrawer : public IFamilyDrawer {
 public:
  explicit FamilyDrawer(std::shared_ptr<Family> family)
      : family_{std::move(family)} {}

  auto GetLabel() const -> std::string override { return "Coupler 1x2"; }

  auto GetColor() const -> ImColor override { return {255, 0, 255}; }

 private:
  std::shared_ptr<Family> family_{};
};

auto CreateFamilyDrawer(std::shared_ptr<Family> family)
    -> std::unique_ptr<IFamilyDrawer> {
  return std::make_unique<FamilyDrawer>(std::move(family));
}
}  // namespace

auto CreateCouplerFamily() -> std::shared_ptr<IFamily> {
  return std::make_unique<Family>();
}
}  // namespace esc