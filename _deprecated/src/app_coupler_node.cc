#include "app_coupler_node.h"

#include <memory>

#include "core_i_node.h"
#include "core_id_generator.h"
#include "coreui_empty_pin_drawer.h"
#include "coreui_flow_input_pin_drawer.h"
#include "coreui_flow_output_pin_drawer.h"
#include "coreui_i_family_traits.h"
#include "coreui_i_node_traits.h"
#include "coreui_i_pin_traits.h"
#include "cpp_assert.h"
#include "crude_json.h"
#include "frame_node.h"
#include "imgui_node_editor.h"
#include "json_i_family_writer.h"
#include "json_i_node_parser.h"
#include "json_i_node_writer.h"

namespace esc {
namespace {
class Node;
class Family;

constexpr auto kTypeName = "CouplerNode";

auto CreateNodeWriter(std::shared_ptr<Node> node)
    -> std::unique_ptr<json::INodeWriter>;
auto CreateNodeDrawer(std::shared_ptr<Node> node)
    -> std::unique_ptr<coreui::INodeTraits>;
auto CreateFamilyWriter(std::shared_ptr<Family> family)
    -> std::unique_ptr<json::IFamilyWriter>;
auto CreateFamilyDrawer(std::shared_ptr<Family> family)
    -> std::unique_ptr<coreui::IFamilyTraits>;

// NOLINTNEXTLINE(*-multiple-inheritance)
class Node : public core::INode, public std::enable_shared_from_this<Node> {
 public:
  Node(ne::NodeId id, std::vector<ne::PinId> pin_ids, int percentage_index = {})
      : INode{id, std::move(pin_ids)}, percentage_index_{percentage_index} {}

  auto CreateWriter() -> std::unique_ptr<json::INodeWriter> override {
    return CreateNodeWriter(shared_from_this());
  }

  auto CreateUiTraits() -> std::unique_ptr<coreui::INodeTraits> override {
    return CreateNodeDrawer(shared_from_this(), state);
  }

  auto GetSmallDrop() const {
    switch (percentage_index_) {
      case 0:
        return -0.4F;
      case 1:
        return -0.7F;
      case 2:
        return -0.95F;
      case 3:
        return -1.2F;
      case 4:
        return -1.55F;
      case 5:
        return -1.85F;
      case 6:
        return -2.2F;
      case 7:
        return -2.6F;
      case 8:
        return -3.0F;
      default:
        return -3.4F;
    };
  }

  auto GetBigDrop() const {
    switch (percentage_index_) {
      case 0:
        return -13.8F;
      case 1:
        return -10.6F;
      case 2:
        return -8.8F;
      case 3:
        return -7.5F;
      case 4:
        return -6.5F;
      case 5:
        return -5.7F;
      case 6:
        return -5.0F;
      case 7:
        return -4.4F;
      case 8:
        return -3.9F;
      default:
        return -3.4F;
    };
  }

  void SetInitialFlowValues(flow::NodeFlow& node_flow) const {
    node_flow.output_pin_flows.at(0) = GetSmallDrop();
    node_flow.output_pin_flows.at(1) = GetBigDrop();
  }

  int percentage_index_{};
};

class NodeParser : public json::INodeParser {
 public:
  explicit NodeParser(int percentage_index)
      : percentage_index_{percentage_index} {}

 private:
  auto ParseFromJson(ne::NodeId parsed_id,
                     std::vector<ne::PinId> parsed_pin_ids,
                     const crude_json::value& json) const
      -> std::shared_ptr<core::INode> override {
    return std::make_shared<Node>(parsed_id, std::move(parsed_pin_ids),
                                  percentage_index_);
  }

  int percentage_index_{};
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

class SmallDropPinDrawer : public coreui::IPinTraits {
 public:
  explicit SmallDropPinDrawer(float drop) : drop_{drop} {}

  auto GetKind() const -> std::optional<ne::PinKind> override {
    return ne::PinKind::Input;
  }

  auto GetFloat() -> std::optional<float*> override { return &drop_; }

  auto IsEditable() const -> bool override { return false; }

 private:
  float drop_{};
};

class BigDropPinDrawer : public coreui::IPinTraits {
 public:
  explicit BigDropPinDrawer(float drop) : drop_{drop} {}

  auto GetKind() const -> std::optional<ne::PinKind> override {
    return ne::PinKind::Input;
  }

  auto GetFloat() -> std::optional<float*> override { return &drop_; }

  auto IsEditable() const -> bool override { return false; }

 private:
  float drop_{};
};

class NodeDrawer : public coreui::INodeTraits {
 public:
  explicit NodeDrawer(std::shared_ptr<Node> node)
      : node_{std::move(node)},
        flow_pin_values_{
            state.core_state->flow_calculator_.GetCalculatedFlow(*node_)} {}

  auto GetLabel() const -> std::string override {
    return CouplerNode::CreateFamily(node_->percentage_index_)
        ->CreateUiTraits()
        ->GetLabel();
  }

  auto GetColor() const -> ImColor override {
    return CouplerNode::CreateFamily(node_->percentage_index_)
        ->CreateUiTraits()
        ->GetColor();
  }

  auto CreatePinTraits() const
      -> std::vector<std::unique_ptr<coreui::IPinTraits>> override {
    auto pin_drawers = std::vector<std::unique_ptr<coreui::IPinTraits>>{};

    pin_drawers.emplace_back(
        std::make_unique<coreui::FlowInputPinDrawer>(flow_pin_values_));
    pin_drawers.emplace_back(
        std::make_unique<SmallDropPinDrawer>(node_->GetSmallDrop()));
    pin_drawers.emplace_back(
        std::make_unique<BigDropPinDrawer>(node_->GetBigDrop()));
    pin_drawers.emplace_back(
        std::make_unique<coreui::EmptyPinDrawer>(ne::PinKind::Output));
    pin_drawers.emplace_back(std::make_unique<coreui::FlowOutputPinDrawer>(
        flow_pin_values_, node_->GetOutputPinIds()[0]));

    return pin_drawers;
  }

 private:
  std::shared_ptr<Node> node_{};
  flow::NodeFlow flow_pin_values_{};
};

auto CreateNodeDrawer(std::shared_ptr<Node> node)
    -> std::unique_ptr<coreui::INodeTraits> {
  return std::make_unique<NodeDrawer>(std::move(node), state);
}

// NOLINTNEXTLINE(*-multiple-inheritance)
class Family : public core::IFamily,
               public std::enable_shared_from_this<Family> {
 public:
  explicit Family(int percentage_index,
                  std::vector<std::shared_ptr<core::INode>> nodes = {})
      : IFamily{std::move(nodes)}, percentage_index_{percentage_index} {}

  auto CreateNode(core::IdGenerator& id_generator)
      -> std::shared_ptr<core::INode> override {
    return std::make_shared<Node>(id_generator.Generate<ne::NodeId>(),
                                  id_generator.GenerateN<ne::PinId>(6),
                                  percentage_index_);
  }

  auto CreateNodeParser() -> std::unique_ptr<json::INodeParser> override {
    return std::make_unique<NodeParser>(percentage_index_);
  }

  auto CreateWriter() -> std::unique_ptr<json::IFamilyWriter> override {
    return CreateFamilyWriter(shared_from_this());
  }

  auto CreateUiTraits() -> std::unique_ptr<coreui::IFamilyTraits> override {
    return CreateFamilyDrawer(shared_from_this());
  }

  auto GetPercentageIndex() const { return percentage_index_; }

 private:
  int percentage_index_{};
};

class FamilyParser : public json::IFamilyParser {
 public:
  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto ParseFromJson(std::vector<std::shared_ptr<core::INode>> parsed_nodes,
                     const crude_json::value& json) const
      -> std::shared_ptr<core::IFamily> override {
    const auto percentage_index =
        static_cast<int>(json["percentage_index"].get<crude_json::number>());
    return std::make_shared<Family>(percentage_index, std::move(parsed_nodes));
  }
};

class FamilyWriter : public json::IFamilyWriter {
 public:
  explicit FamilyWriter(std::shared_ptr<Family> family)
      : family_{std::move(family)} {}

  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto WriteToJson() const -> crude_json::value override {
    auto json = crude_json::value{};
    json["percentage_index"] =
        static_cast<crude_json::number>(family_->GetPercentageIndex());
    return json;
  }

 private:
  std::shared_ptr<Family> family_{};
};

auto CreateFamilyWriter(std::shared_ptr<Family> family)
    -> std::unique_ptr<json::IFamilyWriter> {
  return std::make_unique<FamilyWriter>(std::move(family));
}

class FamilyDrawer : public coreui::IFamilyTraits {
 public:
  explicit FamilyDrawer(std::shared_ptr<Family> family)
      : family_{std::move(family)} {}

  auto GetLabel() const -> std::string override {
    const auto percentage = (family_->GetPercentageIndex() + 1) * 5;
    return GetGroupLabel() + " " + std::to_string(percentage) + "%-" +
           std::to_string(100 - percentage) + "%";
  }

  auto GetColor() const -> ImColor override { return {255, 0, 255}; }

  auto GetGroupLabel() const -> std::string override { return "Coupler"; }

 private:
  std::shared_ptr<Family> family_{};
};

auto CreateFamilyDrawer(std::shared_ptr<Family> family)
    -> std::unique_ptr<coreui::IFamilyTraits> {
  return std::make_unique<FamilyDrawer>(std::move(family));
}
}  // namespace

auto CouplerNode::CreateFamily(int percentage_index)
    -> std::shared_ptr<core::IFamily> {
  return std::make_unique<Family>(percentage_index);
}

auto CouplerNode::CreateFamilyParser() -> std::unique_ptr<json::IFamilyParser> {
  return std::make_unique<FamilyParser>();
}
}  // namespace esc