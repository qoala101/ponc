#include "app_splitter_node.h"

#include <memory>
#include <string>
#include <vector>

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

constexpr auto kTypeName = "SplitterNode";

auto CreateNodeWriter(std::shared_ptr<Node> node)
    -> std::unique_ptr<json::INodeWriter>;
auto CreateNodeDrawer(std::shared_ptr<Node> node)
    -> std::unique_ptr<coreui::INodeDrawer>;
auto CreateFamilyWriter(std::shared_ptr<Family> family)
    -> std::unique_ptr<json::IFamilyWriter>;
auto CreateFamilyDrawer(std::shared_ptr<Family> family)
    -> std::unique_ptr<coreui::IFamilyDrawer>;

// NOLINTNEXTLINE(*-multiple-inheritance)
class Node : public core::INode, public std::enable_shared_from_this<Node> {
 public:
  Node(ne::NodeId id, std::vector<ne::PinId> pin_ids)
      : INode{id, std::move(pin_ids)} {}

  auto CreateWriter() -> std::unique_ptr<json::INodeWriter> override {
    return CreateNodeWriter(shared_from_this());
  }

  auto CreateDrawer() -> std::unique_ptr<coreui::INodeDrawer> override {
    return CreateNodeDrawer(shared_from_this(), state);
  }

  auto GetNumOutputs() const { return GetOutputPinIds().size(); }

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

  void SetInitialFlowValues(flow::NodeFlow& node_flow) const {
    const auto drop = GetDrop();

    for (auto& [pin_id, flow] : node_flow.output_pin_flows) {
      flow = drop;
    }
  }
};

class NodeParser : public json::INodeParser {
 private:
  auto ParseFromJson(ne::NodeId parsed_id,
                     std::vector<ne::PinId> parsed_pin_ids,
                     const crude_json::value& json) const
      -> std::shared_ptr<core::INode> override {
    return std::make_shared<Node>(parsed_id, std::move(parsed_pin_ids));
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

class DropPinDrawer : public coreui::IPinDrawer {
 public:
  explicit DropPinDrawer(float drop) : drop_{drop} {}

  auto GetLabel [[nodiscard]] () const -> std::string override { return {}; }

  auto GetKind [[nodiscard]] () const -> std::optional<ne::PinKind> override {
    return ne::PinKind::Input;
  }

  auto GetFloat [[nodiscard]] () -> std::optional<float*> override {
    return &drop_;
  }

  auto IsEditable [[nodiscard]] () const -> bool override { return false; }

 private:
  float drop_{};
};

class NodeDrawer : public coreui::INodeDrawer {
 public:
  explicit NodeDrawer(std::shared_ptr<Node> node)
      : node_{std::move(node)},
        flow_pin_values_{
            state.core_state->flow_calculator_.GetCalculatedFlow(*node_)} {}

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

  auto CreatePinDrawers [[nodiscard]] () const
      -> std::vector<std::unique_ptr<coreui::IPinDrawer>> override {
    auto pin_drawers = std::vector<std::unique_ptr<coreui::IPinDrawer>>();

    pin_drawers.emplace_back(
        std::make_unique<coreui::FlowInputPinDrawer>(flow_pin_values_));
    pin_drawers.emplace_back(std::make_unique<DropPinDrawer>(node_->GetDrop()));

    for (const auto pin_flow : flow_pin_values_.output_pin_flows) {
      pin_drawers.emplace_back(std::make_unique<coreui::FlowOutputPinDrawer>(
          flow_pin_values_, pin_flow.first));
    }

    return pin_drawers;
  }

 private:
  std::shared_ptr<Node> node_{};
  flow::NodeFlow flow_pin_values_{};
};

auto CreateNodeDrawer(std::shared_ptr<Node> node)
    -> std::unique_ptr<coreui::INodeDrawer> {
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

  auto CreateDrawer() -> std::unique_ptr<coreui::IFamilyDrawer> override {
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

class FamilyDrawer : public coreui::IFamilyDrawer {
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
    -> std::unique_ptr<coreui::IFamilyDrawer> {
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
}  // namespace esc