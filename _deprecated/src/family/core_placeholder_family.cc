#include "core_placeholder_family.h"

#include <memory>
#include <ranges>
#include <string>
#include <vector>

#include "core_i_family.h"
#include "core_i_node.h"
#include "core_id_generator.h"
#include "core_link.h"
#include "coreui_flow_input_pin_drawer.h"
#include "coreui_flow_output_pin_drawer.h"
#include "coreui_i_family_drawer.h"
#include "coreui_i_node_drawer.h"
#include "coreui_i_pin_drawer.h"
#include "cpp_assert.h"
#include "crude_json.h"
#include "frame_node.h"
#include "imgui_node_editor.h"
#include "json_i_family_writer.h"
#include "json_i_node_parser.h"
#include "json_i_node_writer.h"

namespace esc::core {
namespace {
class Node;

constexpr auto kTypeName = "PlaceholderNode";

auto CreateNodeWriter(std::shared_ptr<Node> node)
    -> std::unique_ptr<json::INodeWriter>;
auto CreateNodeDrawer(std::shared_ptr<Node> node)
    -> std::unique_ptr<coreui::INodeDrawer>;
auto CreateFamilyWriter(std::shared_ptr<PlaceholderFamily> family)
    -> std::unique_ptr<json::IFamilyWriter>;
auto CreateFamilyDrawer(std::shared_ptr<PlaceholderFamily> family)
    -> std::unique_ptr<coreui::IFamilyDrawer>;

// NOLINTNEXTLINE(*-multiple-inheritance)
class Node : public INode, public std::enable_shared_from_this<Node> {
 public:
  Node(ne::NodeId id, std::vector<ne::PinId> pin_ids, bool has_input_pin)
      : INode{id, std::move(pin_ids)}, has_input_pin_{has_input_pin} {}

  auto CreateWriter() -> std::unique_ptr<json::INodeWriter> override {
    return CreateNodeWriter(shared_from_this());
  }

  auto CreateDrawer() -> std::unique_ptr<coreui::INodeDrawer> override {
    return CreateNodeDrawer(shared_from_this(), state);
  }

  bool has_input_pin_{};
};

class NodeParser : public json::INodeParser {
 private:
  auto ParseFromJson(ne::NodeId parsed_id,
                     std::vector<ne::PinId> parsed_pin_ids,
                     const crude_json::value& json) const
      -> std::shared_ptr<INode> override {
    return std::make_shared<Node>(
        parsed_id, std::move(parsed_pin_ids),
        json["has_input_pin"].get<crude_json::boolean>());
  }
};

class NodeWriter : public json::INodeWriter {
 public:
  explicit NodeWriter(std::shared_ptr<Node> node) : node_{std::move(node)} {}

 private:
  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto WriteToJson() const -> crude_json::value override {
    auto json = crude_json::value{};
    json["has_input_pin"] =
        static_cast<crude_json::boolean>(node_->has_input_pin_);
    return json;
  }

  std::shared_ptr<Node> node_{};
};

auto CreateNodeWriter(std::shared_ptr<Node> node)
    -> std::unique_ptr<json::INodeWriter> {
  return std::make_unique<NodeWriter>(std::move(node));
}

class NodeDrawer : public coreui::INodeDrawer {
 public:
  explicit NodeDrawer(std::shared_ptr<Node> node)
      : node_{std::move(node)},
        flow_pin_values_{
            state.core_state->flow_calculator_.GetCalculatedFlow(*node_)} {}

  auto GetLabel() const -> std::string override {
    return std::make_shared<PlaceholderFamily>()->CreateDrawer()->GetLabel();
  }

  auto GetColor() const -> ImColor override {
    return std::make_shared<PlaceholderFamily>()->CreateDrawer()->GetColor();
  }

  auto CreatePinDrawers() const
      -> std::vector<std::unique_ptr<coreui::IPinDrawer>> override {
    auto pin_drawers = std::vector<std::unique_ptr<coreui::IPinDrawer>>{};

    if (node_->has_input_pin_) {
      pin_drawers.emplace_back(
          std::make_unique<coreui::FlowInputPinDrawer>(flow_pin_values_));
    }

    for (const auto pin_id : node_->GetOutputPinIds()) {
      pin_drawers.emplace_back(std::make_unique<coreui::FlowOutputPinDrawer>(
          flow_pin_values_, pin_id));
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
}  // namespace

PlaceholderFamily::PlaceholderFamily(std::vector<std::shared_ptr<INode>> nodes)
    : IFamily{std::move(nodes)} {}

auto PlaceholderFamily::CreateNode(IdGenerator& id_generator)
    -> std::shared_ptr<INode> {
  Expects(false);
  return {};
}

auto PlaceholderFamily::CreateNodeParser()
    -> std::unique_ptr<json::INodeParser> {
  return std::make_unique<NodeParser>();
}

auto PlaceholderFamily::CreateWriter() -> std::unique_ptr<json::IFamilyWriter> {
  return CreateFamilyWriter(shared_from_this());
}

auto PlaceholderFamily::CreateDrawer()
    -> std::unique_ptr<coreui::IFamilyDrawer> {
  return CreateFamilyDrawer(shared_from_this());
}

auto PlaceholderFamily::EmplaceNodeFromFlow(
    IdGenerator& id_generator, const flow::NodeFlow& connected_flow)
    -> const std::shared_ptr<INode>& {
  // auto pin_ids = std::vector<ne::PinId>{};
  const auto has_input_pin = connected_flow.input_pin_flow.has_value();
  auto num_pins = static_cast<int>(connected_flow.output_pin_flows.size());

  if (has_input_pin) {
    num_pins += 1;
  }

  return EmplaceNode(std::make_shared<Node>(
      id_generator.GetNext<ne::NodeId>(),
      id_generator.GetNextN<ne::PinId>(num_pins), has_input_pin));
}

namespace {
class FamilyParser : public json::IFamilyParser {
 public:
  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto ParseFromJson(std::vector<std::shared_ptr<INode>> parsed_nodes,
                     const crude_json::value& json) const
      -> std::shared_ptr<IFamily> override {
    return std::make_shared<PlaceholderFamily>(std::move(parsed_nodes));
  }
};

class FamilyWriter : public json::IFamilyWriter {
 public:
  explicit FamilyWriter(std::shared_ptr<PlaceholderFamily> family)
      : family_{std::move(family)} {}

  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto WriteToJson() const -> crude_json::value override { return {}; }

 private:
  std::shared_ptr<PlaceholderFamily> family_{};
};

auto CreateFamilyWriter(std::shared_ptr<PlaceholderFamily> family)
    -> std::unique_ptr<json::IFamilyWriter> {
  return std::make_unique<FamilyWriter>(std::move(family));
}

class FamilyDrawer : public coreui::IFamilyDrawer {
 public:
  explicit FamilyDrawer(std::shared_ptr<PlaceholderFamily> family)
      : family_{std::move(family)} {}

  auto GetLabel() const -> std::string override { return "Placeholder"; }

  auto GetColor() const -> ImColor override { return {255, 255, 255}; }

  auto IsUserAccessible() -> bool { return false; }

 private:
  std::shared_ptr<PlaceholderFamily> family_{};
};

auto CreateFamilyDrawer(std::shared_ptr<PlaceholderFamily> family)
    -> std::unique_ptr<coreui::IFamilyDrawer> {
  return std::make_unique<FamilyDrawer>(std::move(family));
}
}  // namespace

auto PlaceholderFamily::CreateParser() -> std::unique_ptr<json::IFamilyParser> {
  return std::make_unique<FamilyParser>();
}
}  // namespace esc::core