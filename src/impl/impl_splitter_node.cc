#include "impl_splitter_node.h"

#include <memory>
#include <string>
#include <vector>

#include "core_i_node.h"
#include "core_id_generator.h"
#include "cpp_assert.h"
#include "crude_json.h"
#include "draw_flow_input_pin_drawer.h"
#include "draw_flow_output_pin_drawer.h"
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

constexpr auto kTypeName = "SplitterNode";

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
  Node(ne::NodeId id, std::vector<ne::PinId> pin_ids)
      : INode{id, std::move(pin_ids)} {}

  auto CreateWriter() -> std::unique_ptr<json::INodeWriter> override {
    return CreateNodeWriter(shared_from_this());
  }

  auto CreateDrawer(const State& state)
      -> std::unique_ptr<draw::INodeDrawer> override {
    return CreateNodeDrawer(shared_from_this(), state);
  }

  auto GetNumOutputs() const {
    return static_cast<int>(GetPinIds().size()) - 2;
  }

  auto GetDrop() const {
    switch (GetNumOutputs()) {
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
  }

  auto GetInitialFlow [[nodiscard]] () const -> core::Flow override {
    const auto& pin_ids = GetPinIds();
    auto flow_values =
        core::Flow{.input_pin_flow = std::pair{pin_ids[0].Get(), float{}}};
    const auto drop = GetDrop();

    for (auto i = 0; i < static_cast<int>(pin_ids.size() - 2); ++i) {
      flow_values.output_pin_flows.emplace(pin_ids[i + 2], drop);
    }

    return flow_values;
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
  explicit DropPinDrawer(float drop) : drop_{drop} {}

  auto GetLabel [[nodiscard]] () const -> std::string override { return {}; }

  auto GetKind [[nodiscard]] () const -> ne::PinKind override {
    return ne::PinKind::Input;
  }

  auto GetFloat [[nodiscard]] () -> float* override { return &drop_; }

  auto IsEditable [[nodiscard]] () const -> bool override { return false; }

 private:
  float drop_{};
};

class NodeDrawer : public draw::INodeDrawer {
 public:
  explicit NodeDrawer(std::shared_ptr<Node> node, const State& state)
      : node_{std::move(node)},
        flow_pin_values_{state.flow_calculator_.GetCalculatedFlow(*node_)} {}

  auto GetLabel() const -> std::string override {
    return SplitterNode::CreateFamily(node_->GetNumOutputs())
        ->CreateDrawer()
        ->GetLabel();
  }

  auto GetColor() const -> ImColor override {
    return SplitterNode::CreateFamily(node_->GetNumOutputs())
        ->CreateDrawer()
        ->GetColor();
  }

  auto CreatePinDrawer(ne::PinId pin_id) const
      -> std::unique_ptr<draw::IPinDrawer> override {
    const auto pin_index = node_->GetPinIndex(pin_id);

    if (pin_index == 0) {
      return std::make_unique<draw::FlowInputPinDrawer>(flow_pin_values_);
    }

    if (pin_index == 1) {
      return std::make_unique<DropPinDrawer>(node_->GetDrop());
    }

    return std::make_unique<draw::FlowOutputPinDrawer>(flow_pin_values_,
                                                       pin_id);
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
  explicit Family(int num_outputs,
                  std::vector<std::shared_ptr<core::INode>> nodes = {})
      : IFamily{std::move(nodes)}, num_outputs_{num_outputs} {}

  auto CreateNode(core::IdGenerator& id_generator)
      -> std::shared_ptr<core::INode> override {
    return std::make_shared<Node>(
        id_generator.GetNext<ne::NodeId>(),
        id_generator.GetNextN<ne::PinId>(2 + num_outputs_));
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

  auto GetNumOutputs() const { return num_outputs_; }

 private:
  int num_outputs_{};
};

class FamilyParser : public json::IFamilyParser {
 public:
  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto ParseFromJson(std::vector<std::shared_ptr<core::INode>> parsed_nodes,
                     const crude_json::value& json) const
      -> std::shared_ptr<core::IFamily> override {
    const auto num_outputs =
        static_cast<int>(json["num_outputs"].get<crude_json::number>());
    return std::make_shared<Family>(num_outputs, std::move(parsed_nodes));
  }
};

class FamilyWriter : public json::IFamilyWriter {
 public:
  explicit FamilyWriter(std::shared_ptr<Family> family)
      : family_{std::move(family)} {}

  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto WriteToJson() const -> crude_json::value override {
    auto json = crude_json::value{};
    json["num_outputs"] =
        static_cast<crude_json::number>(family_->GetNumOutputs());
    return json;
  }

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

  auto GetLabel() const -> std::string override {
    return GetGroupLabel() + " 1x" + std::to_string(family_->GetNumOutputs());
  }

  auto GetColor() const -> ImColor override {
    return {0, 0, 127 + 128 / family_->GetNumOutputs()};
  }

  auto GetGroupLabel() const -> std::string override { return "Splitter"; }

 private:
  std::shared_ptr<Family> family_{};
};

auto CreateFamilyDrawer(std::shared_ptr<Family> family)
    -> std::unique_ptr<draw::IFamilyDrawer> {
  return std::make_unique<FamilyDrawer>(std::move(family));
}
}  // namespace

auto SplitterNode::CreateFamily(int num_outputs)
    -> std::shared_ptr<core::IFamily> {
  return std::make_unique<Family>(num_outputs);
}

auto SplitterNode::CreateFamilyParser()
    -> std::unique_ptr<json::IFamilyParser> {
  return std::make_unique<FamilyParser>();
}
}  // namespace esc::impl