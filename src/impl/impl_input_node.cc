#include "impl_input_node.h"

#include <memory>

#include "core_i_node.h"
#include "core_id_generator.h"
#include "cpp_assert.h"
#include "crude_json.h"
#include "draw_flow_input_pin_drawer.h"
#include "draw_i_family_drawer.h"
#include "draw_i_node_drawer.h"
#include "draw_i_pin_drawer.h"
#include "esc_state.h"
#include "imgui_node_editor.h"
#include "json_i_family_writer.h"
#include "json_i_node_parser.h"
#include "json_i_node_writer.h"

namespace esc::impl {
namespace {
class Node;
class Family;

constexpr auto kTypeName = "InputNode";

auto CreateNodeWriter(std::shared_ptr<Node> node)
    -> std::unique_ptr<json::INodeWriter>;
auto CreateNodeDrawer(std::shared_ptr<Node> node, const State& state)
    -> std::unique_ptr<draw::INodeDrawer>;
auto CreateFamilyWriter(std::shared_ptr<Family> family)
    -> std::unique_ptr<json::IFamilyWriter>;
auto CreateFamilyDrawer(std::shared_ptr<Family> family)
    -> std::unique_ptr<draw::IFamilyDrawer>;

// NOLINTNEXTLINE(*-multiple-inheritance)
class Node : public core::INode, public std::enable_shared_from_this<Node> {
 public:
  Node(ne::NodeId id, std::vector<ne::PinId> pin_ids, float value = 6.0F)
      : INode{id, std::move(pin_ids)}, value_{value} {}

  auto CreateWriter() -> std::unique_ptr<json::INodeWriter> override {
    return CreateNodeWriter(shared_from_this());
  }

  auto CreateDrawer(const State& state)
      -> std::unique_ptr<draw::INodeDrawer> override {
    return CreateNodeDrawer(shared_from_this(), state);
  }

  auto GetInitialFlow [[nodiscard]] () const -> core::Flow override {
    return {.output_pin_flows = {{GetPinIds()[0].Get(), value_}}};
  }

  float value_{};
};

class NodeParser : public json::INodeParser {
 private:
  auto ParseFromJson(ne::NodeId parsed_node_id,
                     std::vector<ne::PinId> parsed_pin_ids,
                     const crude_json::value& json) const
      -> std::shared_ptr<core::INode> override {
    return std::make_shared<Node>(parsed_node_id, std::move(parsed_pin_ids),
                                  json["value"].get<crude_json::number>());
  }
};

class NodeWriter : public json::INodeWriter {
 public:
  explicit NodeWriter(std::shared_ptr<Node> node) : node_{std::move(node)} {}

 private:
  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto WriteToJson() const -> crude_json::value override {
    auto json = crude_json::value{};
    json["value"] = static_cast<crude_json::number>(node_->value_);
    return json;
  }

  std::shared_ptr<Node> node_{};
};

auto CreateNodeWriter(std::shared_ptr<Node> node)
    -> std::unique_ptr<json::INodeWriter> {
  return std::make_unique<NodeWriter>(std::move(node));
}

class PinDrawer : public draw::IPinDrawer {
 public:
  explicit PinDrawer(std::shared_ptr<Node> node) : node_{std::move(node)} {}

  auto GetLabel [[nodiscard]] () const -> std::string override { return {}; }

  auto GetKind [[nodiscard]] () const -> ne::PinKind override {
    return ne::PinKind::Output;
  }

  auto GetFloat [[nodiscard]] () -> float* override { return &node_->value_; }

  auto IsEditable [[nodiscard]] () const -> bool override { return true; }

 private:
  std::shared_ptr<Node> node_{};
};

class NodeDrawer : public draw::INodeDrawer {
 public:
  explicit NodeDrawer(std::shared_ptr<Node> node, const State& state)
      : node_{std::move(node)},
        flow_pin_values_{
            state.flow_calculator_.GetCalculatedFlow(*node_)} {}

  auto GetLabel() const -> std::string override {
    return InputNode::CreateFamily()->CreateDrawer()->GetLabel();
  }

  auto GetColor() const -> ImColor override {
    return InputNode::CreateFamily()->CreateDrawer()->GetColor();
  }

  auto CreatePinDrawer(ne::PinId pin_id) const
      -> std::unique_ptr<draw::IPinDrawer> override {
    return std::make_unique<PinDrawer>(node_);
  }

 private:
  std::shared_ptr<Node> node_{};
  core::Flow flow_pin_values_{};
};

auto CreateNodeDrawer(std::shared_ptr<Node> node, const State& state)
    -> std::unique_ptr<draw::INodeDrawer> {
  return std::make_unique<NodeDrawer>(std::move(node), state);
}

// NOLINTNEXTLINE(*-multiple-inheritance)
class Family : public core::IFamily,
               public std::enable_shared_from_this<Family> {
 public:
  explicit Family(std::vector<std::shared_ptr<core::INode>> nodes = {})
      : IFamily{std::move(nodes)} {}

  auto CreateNode(core::IdGenerator& id_generator)
      -> std::shared_ptr<core::INode> override {
    return std::make_shared<Node>(id_generator.GetNext<ne::NodeId>(),
                                  id_generator.GetNextN<ne::PinId>(1));
  }

  auto CreateNodeParser() -> std::unique_ptr<json::INodeParser> override {
    return std::make_unique<NodeParser>();
  }

  auto CreateWriter() -> std::unique_ptr<json::IFamilyWriter> override {
    return CreateFamilyWriter(shared_from_this());
  }

  auto CreateDrawer() -> std::unique_ptr<draw::IFamilyDrawer> override {
    return CreateFamilyDrawer(shared_from_this());
  }
};

class FamilyParser : public json::IFamilyParser {
 public:
  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto ParseFromJson(std::vector<std::shared_ptr<core::INode>> parsed_nodes,
                     const crude_json::value& json) const
      -> std::shared_ptr<core::IFamily> override {
    return std::make_shared<Family>(std::move(parsed_nodes));
  }
};

class FamilyWriter : public json::IFamilyWriter {
 public:
  explicit FamilyWriter(std::shared_ptr<Family> family)
      : family_{std::move(family)} {}

  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto WriteToJson() const -> crude_json::value override { return {}; }

 private:
  std::shared_ptr<Family> family_{};
};

auto CreateFamilyWriter(std::shared_ptr<Family> family)
    -> std::unique_ptr<json::IFamilyWriter> {
  return std::make_unique<FamilyWriter>(std::move(family));
}

class FamilyDrawer : public draw::IFamilyDrawer {
 public:
  explicit FamilyDrawer(std::shared_ptr<Family> family)
      : family_{std::move(family)} {}

  auto GetLabel() const -> std::string override { return "Input"; }

  auto GetColor() const -> ImColor override { return {255, 0, 0}; }

 private:
  std::shared_ptr<Family> family_{};
};

auto CreateFamilyDrawer(std::shared_ptr<Family> family)
    -> std::unique_ptr<draw::IFamilyDrawer> {
  return std::make_unique<FamilyDrawer>(std::move(family));
}
}  // namespace

auto InputNode::CreateFamily() -> std::shared_ptr<core::IFamily> {
  return std::make_unique<Family>();
}

auto InputNode::CreateFamilyParser() -> std::unique_ptr<json::IFamilyParser> {
  return std::make_unique<FamilyParser>();
}
}  // namespace esc::impl