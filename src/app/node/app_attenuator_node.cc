#include "app_attenuator_node.h"

#include <cstdint>
#include <memory>

#include "core_i_node.h"
#include "core_id_generator.h"
#include "coreui_flow_pin_traits.h"
#include "coreui_i_family_traits.h"
#include "coreui_i_node_traits.h"
#include "coreui_i_pin_traits.h"
#include "coreui_project.h"
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
auto CreateNodeDrawer(std::shared_ptr<Node> node)
    -> std::unique_ptr<coreui::INodeTraits>;
auto CreateFamilyWriter(std::shared_ptr<Family> family)
    -> std::unique_ptr<json::IFamilyWriter>;
auto CreateFamilyDrawer(std::shared_ptr<Family> family)
    -> std::unique_ptr<coreui::IFamilyTraits>;

// NOLINTNEXTLINE(*-multiple-inheritance)
class Node : public core::INode, public std::enable_shared_from_this<Node> {
 public:
  Node(ConstructorArgs args, float drop = -15.0F)
      : INode{std::move(args)}, drop_{drop} {}

  auto CreateWriter() -> std::unique_ptr<json::INodeWriter> override {
    return CreateNodeWriter(shared_from_this());
  }

  auto CreateUiTraits() -> std::unique_ptr<coreui::INodeTraits> override {
    return CreateNodeDrawer(shared_from_this());
  }

  void SetInitialFlowValues(flow::NodeFlow& node_flow) const override {
    for (const auto output_pin_id : GetOutputPinIds()) {
      node_flow.output_pin_flows.at(output_pin_id.Get()) = drop_;
    }
  }

  float drop_{};
};

class NodeParser : public json::INodeParser {
 private:
  auto ParseFromJson(core::INode::ConstructorArgs parsed_args,
                     const crude_json::value& json) const
      -> std::shared_ptr<core::INode> override {
    return std::make_shared<Node>(std::move(parsed_args),
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

class DropPinDrawer : public coreui::IPinTraits {
 public:
  explicit DropPinDrawer(std::shared_ptr<Node> node) : node_{std::move(node)} {}

  auto GetKind() const -> std::optional<ne::PinKind> override {
    return ne::PinKind::Input;
  }

  auto GetFloat() -> std::optional<float*> override { return &node_->drop_; }

  auto IsEditable() const -> bool override { return true; }

 private:
  std::shared_ptr<Node> node_{};
};

class NodeDrawer : public coreui::INodeTraits {
 public:
  explicit NodeDrawer(std::shared_ptr<Node> node) : node_{std::move(node)} {}

  auto GetLabel() const -> std::string override { return "Attenuator"; }

  auto GetColor() const -> ImColor override { return {63, 0, 63}; }

  auto CreatePinTraits() const
      -> std::vector<std::unique_ptr<coreui::IPinTraits>> override {
    auto pin_drawers = std::vector<std::unique_ptr<coreui::IPinTraits>>{};

    pin_drawers.emplace_back(
        std::make_unique<coreui::FlowPinTraits>(*node_->GetInputPinId()));
    pin_drawers.emplace_back(std::make_unique<DropPinDrawer>(node_));
    pin_drawers.emplace_back(
        std::make_unique<coreui::FlowPinTraits>(node_->GetOutputPinIds()[0]));

    return pin_drawers;
  }

 private:
  std::shared_ptr<Node> node_{};
};

auto CreateNodeDrawer(std::shared_ptr<Node> node)
    -> std::unique_ptr<coreui::INodeTraits> {
  return std::make_unique<NodeDrawer>(std::move(node));
}

// NOLINTNEXTLINE(*-multiple-inheritance)
class Family : public core::IFamily,
               public std::enable_shared_from_this<Family> {
 public:
  explicit Family(core::FamilyId id) : IFamily{id} {}

  auto CreateNode(core::IdGenerator& id_generator)
      -> std::shared_ptr<core::INode> override {
    return std::make_shared<Node>(core::INode::ConstructorArgs{
        .id = id_generator.Generate<ne::NodeId>(),
        .family_id = GetId(),
        .input_pin_id = id_generator.Generate<ne::PinId>(),
        .output_pin_ids = id_generator.GenerateN<ne::PinId>(1)});
  }

  auto CreateNodeParser() -> std::unique_ptr<json::INodeParser> override {
    return std::make_unique<NodeParser>();
  }

  auto CreateWriter() -> std::unique_ptr<json::IFamilyWriter> override {
    return CreateFamilyWriter(shared_from_this());
  }

  auto CreateUiTraits() -> std::unique_ptr<coreui::IFamilyTraits> override {
    return CreateFamilyDrawer(shared_from_this());
  }
};

class FamilyParser : public json::IFamilyParser {
 public:
  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto ParseFromJson(core::FamilyId parsed_id,
                     const crude_json::value& json) const
      -> std::shared_ptr<core::IFamily> override {
    return std::make_shared<Family>(parsed_id);
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

class FamilyDrawer : public coreui::IFamilyTraits {
 public:
  explicit FamilyDrawer(std::shared_ptr<Family> family)
      : family_{std::move(family)} {}

  auto GetLabel() const -> std::string override { return "Attenuator"; }

  auto GetColor() const -> ImColor override { return {63, 0, 63}; }

 private:
  std::shared_ptr<Family> family_{};
};

auto CreateFamilyDrawer(std::shared_ptr<Family> family)
    -> std::unique_ptr<coreui::IFamilyTraits> {
  return std::make_unique<FamilyDrawer>(std::move(family));
}
}  // namespace

auto AttenuatorNode::CreateFamily(core::FamilyId id)
    -> std::shared_ptr<core::IFamily> {
  return std::make_shared<Family>(id);
}

auto AttenuatorNode::CreateFamilyParser()
    -> std::unique_ptr<json::IFamilyParser> {
  return std::make_unique<FamilyParser>();
}
}  // namespace esc