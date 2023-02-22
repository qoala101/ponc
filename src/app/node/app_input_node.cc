#include "app_input_node.h"

#include <memory>
#include <vector>

#include "app_state.h"
#include "core_i_node.h"
#include "core_id_generator.h"
#include "coreui_flow_pin_drawer.h"
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

constexpr auto kTypeName = "InputNode";

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
  Node(ConstructorArgs args, float value = {6.F})
      : INode{std::move(args)}, value_{value} {}

  auto CreateWriter() -> std::unique_ptr<json::INodeWriter> override {
    return CreateNodeWriter(shared_from_this());
  }

  auto CreateDrawer() -> std::unique_ptr<coreui::INodeDrawer> override {
    return CreateNodeDrawer(shared_from_this());
  }

  void SetInitialFlowValues(flow::NodeFlow& node_flow) const override {
    node_flow.output_pin_flows.at(GetOutputPinIds()[0].Get()) = value_;
  }

  float value_{};
};

class NodeParser : public json::INodeParser {
 private:
  auto ParseFromJson(core::INode::ConstructorArgs parsed_args,
                     const crude_json::value& json) const
      -> std::shared_ptr<core::INode> override {
    return std::make_shared<Node>(std::move(parsed_args),
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

class PinDrawer : public coreui::IPinDrawer {
 public:
  explicit PinDrawer(std::shared_ptr<Node> node) : node_{std::move(node)} {}

  auto GetLabel [[nodiscard]] () const -> std::string override { return {}; }

  auto GetKind [[nodiscard]] () const -> std::optional<ne::PinKind> override {
    return ne::PinKind::Output;
  }

  auto GetFloat [[nodiscard]] () -> std::optional<float*> override {
    return &node_->value_;
  }

  auto IsEditable [[nodiscard]] () const -> bool override { return true; }

  auto GetPinId [[nodiscard]] () const -> std::optional<ne::PinId> {
    return node_->GetOutputPinIds()[0];
  }

 private:
  std::shared_ptr<Node> node_{};
};

class NodeDrawer : public coreui::INodeDrawer {
 public:
  explicit NodeDrawer(std::shared_ptr<Node> node) : node_{std::move(node)} {}

  auto GetLabel() const -> std::string override { return "Input"; }

  auto GetColor() const -> ImColor override { return {255, 0, 0}; }

  auto CreatePinDrawers() const
      -> std::vector<std::unique_ptr<coreui::IPinDrawer>> override {
    auto pin_drawers = std::vector<std::unique_ptr<coreui::IPinDrawer>>{};

    pin_drawers.emplace_back(std::make_unique<PinDrawer>(node_));

    return pin_drawers;
  }

 private:
  std::shared_ptr<Node> node_{};
};

auto CreateNodeDrawer(std::shared_ptr<Node> node)
    -> std::unique_ptr<coreui::INodeDrawer> {
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
        .id = id_generator.GetNext<ne::NodeId>(),
        .family_id = GetId(),
        .output_pin_ids = id_generator.GetNextN<ne::PinId>(1)});
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

class FamilyDrawer : public coreui::IFamilyDrawer {
 public:
  explicit FamilyDrawer(std::shared_ptr<Family> family)
      : family_{std::move(family)} {}

  auto GetLabel() const -> std::string override { return "Input"; }

  auto GetColor() const -> ImColor override { return {255, 0, 0}; }

 private:
  std::shared_ptr<Family> family_{};
};

auto CreateFamilyDrawer(std::shared_ptr<Family> family)
    -> std::unique_ptr<coreui::IFamilyDrawer> {
  return std::make_unique<FamilyDrawer>(std::move(family));
}
}  // namespace

auto InputNode::CreateFamily(core::FamilyId id)
    -> std::shared_ptr<core::IFamily> {
  return std::make_shared<Family>(id);
}

auto InputNode::CreateFamilyParser() -> std::unique_ptr<json::IFamilyParser> {
  return std::make_unique<FamilyParser>();
}
}  // namespace esc