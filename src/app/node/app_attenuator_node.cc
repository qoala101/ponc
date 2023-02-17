#include <cstdint>
#include <memory>

#include "app_attenuator_node.h"
#include "app_state.h"
#include "core_i_node.h"
#include "core_id_generator.h"
#include "coreui_flow_input_pin_drawer.h"
#include "coreui_flow_output_pin_drawer.h"
#include "coreui_i_family_drawer.h"
#include "coreui_i_node_drawer.h"
#include "coreui_i_pin_drawer.h"
#include "cpp_assert.h"
#include "crude_json.h"
#include "imgui_node_editor.h"
#include "json_i_family_writer.h"
#include "json_i_node_parser.h"
#include "json_i_node_writer.h"

namespace esc {
namespace {
class Node;
class Family;

constexpr auto kTypeName = "AttenuatorNode";

auto CreateNodeWriter(std::shared_ptr<Node> node)
    -> std::unique_ptr<json::INodeWriter>;
auto CreateNodeDrawer(std::shared_ptr<Node> node, const StateNoQueue& state)
    -> std::unique_ptr<coreui::INodeDrawer>;
auto CreateFamilyWriter(std::shared_ptr<Family> family)
    -> std::unique_ptr<json::IFamilyWriter>;
auto CreateFamilyDrawer(std::shared_ptr<Family> family)
    -> std::unique_ptr<coreui::IFamilyDrawer>;

// NOLINTNEXTLINE(*-multiple-inheritance)
class Node : public core::INode, public std::enable_shared_from_this<Node> {
 public:
  Node(ne::NodeId id, std::vector<ne::PinId> pin_ids, float drop = -15.0F)
      : INode{id, std::move(pin_ids)}, drop_{drop} {}

  auto CreateWriter() -> std::unique_ptr<json::INodeWriter> override {
    return CreateNodeWriter(shared_from_this());
  }

  auto CreateDrawer(const StateNoQueue& state)
      -> std::unique_ptr<coreui::INodeDrawer> override {
    return CreateNodeDrawer(shared_from_this(), state);
  }

  auto GetInitialFlow [[nodiscard]] () const -> flow::NodeFlow override {
    const auto& pin_ids = GetPinIds();

    return {.input_pin_flow = std::pair{pin_ids[0].Get(), float{}},
            .output_pin_flows = {{pin_ids[2].Get(), drop_}}};
  }

  float drop_{};
};

class NodeParser : public json::INodeParser {
 private:
  auto ParseFromJson(ne::NodeId parsed_node_id,
                     std::vector<ne::PinId> parsed_pin_ids,
                     const crude_json::value& json) const
      -> std::shared_ptr<core::INode> override {
    return std::make_shared<Node>(parsed_node_id, std::move(parsed_pin_ids),
                                  json["drop"].get<crude_json::number>());
  }
};

class NodeWriter : public json::INodeWriter {
 public:
  explicit NodeWriter(std::shared_ptr<Node> node) : node_{std::move(node)} {}

 private:
  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto WriteToJson() const -> crude_json::value override {
    auto json = crude_json::value{};
    json["drop"] = static_cast<crude_json::number>(node_->drop_);
    return json;
  }

  std::shared_ptr<Node> node_{};
};

auto CreateNodeWriter(std::shared_ptr<Node> node)
    -> std::unique_ptr<json::INodeWriter> {
  return std::make_unique<NodeWriter>(std::move(node));
}

class DropPinDrawer : public coreui::IPinDrawer {
 public:
  explicit DropPinDrawer(std::shared_ptr<Node> node) : node_{std::move(node)} {}

  auto GetKind [[nodiscard]] () const -> ne::PinKind override {
    return ne::PinKind::Input;
  }

  auto GetFloat [[nodiscard]] () -> float* override { return &node_->drop_; }

  auto IsEditable [[nodiscard]] () const -> bool override { return true; }

 private:
  std::shared_ptr<Node> node_{};
};

class NodeDrawer : public coreui::INodeDrawer {
 public:
  explicit NodeDrawer(std::shared_ptr<Node> node, const StateNoQueue& state)
      : node_{std::move(node)},
        flow_pin_values_{
            state.core_state->flow_calculator_.GetCalculatedFlow(*node_)} {}

  auto GetLabel() const -> std::string override {
    return AttenuatorNode::CreateFamily()->CreateDrawer()->GetLabel();
  }

  auto GetColor() const -> ImColor override {
    return AttenuatorNode::CreateFamily()->CreateDrawer()->GetColor();
  }

  auto CreatePinDrawer(ne::PinId pin_id) const
      -> std::unique_ptr<coreui::IPinDrawer> override {
    const auto pin_index = node_->GetPinIndex(pin_id);

    if (pin_index == 0) {
      return std::make_unique<coreui::FlowInputPinDrawer>(flow_pin_values_);
    }

    if (pin_index == 1) {
      return std::make_unique<DropPinDrawer>(node_);
    }

    return std::make_unique<coreui::FlowOutputPinDrawer>(flow_pin_values_,
                                                         pin_id);
  }

 private:
  std::shared_ptr<Node> node_{};
  flow::NodeFlow flow_pin_values_{};
};

auto CreateNodeDrawer(std::shared_ptr<Node> node, const StateNoQueue& state)
    -> std::unique_ptr<coreui::INodeDrawer> {
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
                                  id_generator.GetNextN<ne::PinId>(3));
  }

  auto CreateNodeParser() -> std::unique_ptr<json::INodeParser> override {
    return std::make_unique<NodeParser>();
  }

  auto CreateWriter() -> std::unique_ptr<json::IFamilyWriter> override {
    return CreateFamilyWriter(shared_from_this());
  }

  auto CreateDrawer() -> std::unique_ptr<coreui::IFamilyDrawer> override {
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

class FamilyDrawer : public coreui::IFamilyDrawer {
 public:
  explicit FamilyDrawer(std::shared_ptr<Family> family)
      : family_{std::move(family)} {}

  auto GetLabel() const -> std::string override { return "Attenuator"; }

  auto GetColor() const -> ImColor override { return {64, 0, 64}; }

 private:
  std::shared_ptr<Family> family_{};
};

auto CreateFamilyDrawer(std::shared_ptr<Family> family)
    -> std::unique_ptr<coreui::IFamilyDrawer> {
  return std::make_unique<FamilyDrawer>(std::move(family));
}
}  // namespace

auto AttenuatorNode::CreateFamily() -> std::shared_ptr<core::IFamily> {
  return std::make_unique<Family>();
}

auto AttenuatorNode::CreateFamilyParser()
    -> std::unique_ptr<json::IFamilyParser> {
  return std::make_unique<FamilyParser>();
}
}  // namespace esc