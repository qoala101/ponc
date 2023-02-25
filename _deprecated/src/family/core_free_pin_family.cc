#include "core_free_pin_family.h"

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
#include "coreui_i_family_traits.h"
#include "coreui_i_node_traits.h"
#include "coreui_i_pin_traits.h"
#include "cpp_assert.h"
#include "crude_json.h"
#include "imgui_node_editor.h"
#include "json_i_family_writer.h"
#include "json_i_node_parser.h"
#include "json_i_node_writer.h"

namespace esc::core {
namespace {
class Node;

constexpr auto kTypeName = "FreePinNode";

auto CreateNodeWriter(std::shared_ptr<Node> node)
    -> std::unique_ptr<json::INodeWriter>;
auto CreateNodeDrawer(std::shared_ptr<Node> node)
    -> std::unique_ptr<coreui::INodeTraits>;
auto CreateFamilyWriter(std::shared_ptr<FreePinFamily> family)
    -> std::unique_ptr<json::IFamilyWriter>;
auto CreateFamilyDrawer(std::shared_ptr<FreePinFamily> family)
    -> std::unique_ptr<coreui::IFamilyTraits>;

// NOLINTNEXTLINE(*-multiple-inheritance)
class Node : public INode, public std::enable_shared_from_this<Node> {
 public:
  Node(ne::NodeId id, FamilyId family_id, std::vector<ne::PinId> pin_ids,
       bool has_input_pin)
      : INode{id, family_id, std::move(pin_ids)},
        has_input_pin_{has_input_pin} {}

  auto CreateWriter() -> std::unique_ptr<json::INodeWriter> override {
    return CreateNodeWriter(shared_from_this());
  }

  auto CreateUiTraits() -> std::unique_ptr<coreui::INodeTraits> override {
    return CreateNodeDrawer(shared_from_this());
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

class PinDrawer : public coreui::IPinTraits {
 public:
  explicit PinDrawer(ne::PinKind pin_kind) : pin_kind_{pin_kind} {}

  auto GetLabel() const -> std::string override { return {}; }

  auto GetKind() const -> std::optional<ne::PinKind> override {
    return pin_kind_;
  }

  auto IsEditable() const -> bool override { return false; }

 private:
  ne::PinKind pin_kind_{};
};

class NodeDrawer : public coreui::INodeTraits {
 public:
  explicit NodeDrawer(std::shared_ptr<Node> node)
      : node_{std::move(node)},
        flow_pin_values_{
            state.core_state->flow_calculator_.GetCalculatedFlow(*node_)} {}

  auto GetLabel() const -> std::string override {
    return std::make_shared<FreePinFamily>()->CreateUiTraits()->GetLabel();
  }

  auto GetColor() const -> ImColor override {
    return std::make_shared<FreePinFamily>()->CreateUiTraits()->GetColor();
  }

  auto HasHeader() const -> bool override { return false; }

  auto CreatePinTraits() const
      -> std::vector<std::unique_ptr<coreui::IPinTraits>> override {
    auto pin_drawers = std::vector<std::unique_ptr<coreui::IPinTraits>>{};

    pin_drawers.emplace_back(std::make_unique<PinDrawer>(ne::PinKind::Input));
    pin_drawers.emplace_back(std::make_unique<PinDrawer>(ne::PinKind::Output));

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
}  // namespace

FreePinFamily::FreePinFamily(FamilyId id) : IFamily{id} {}

auto FreePinFamily::CreateNode(IdGenerator& id_generator)
    -> std::unique_ptr<INode> {
  Expects(false);
  return {};
}

auto FreePinFamily::CreateNodeParser() -> std::unique_ptr<json::INodeParser> {
  return std::make_unique<NodeParser>();
}

auto FreePinFamily::CreateWriter() -> std::unique_ptr<json::IFamilyWriter> {
  return CreateFamilyWriter(shared_from_this());
}

auto FreePinFamily::CreateUiTraits() -> std::unique_ptr<coreui::IFamilyTraits> {
  return CreateFamilyDrawer(shared_from_this());
}

auto FreePinFamily::CreateNodeFromFlow(IdGenerator& id_generator,
                                       ne::PinId pin_id, bool has_input_pin)
    -> std::unique_ptr<INode> {
  return std::make_shared<Node>(id_generator.Generate<ne::NodeId>(), GetId(),
                                std::vector<ne::PinId>{pin_id}, has_input_pin);
}

namespace {
class FamilyParser : public json::IFamilyParser {
 public:
  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto ParseFromJson(std::vector<std::shared_ptr<INode>> parsed_nodes,
                     const crude_json::value& json) const
      -> std::shared_ptr<IFamily> override {
    return std::make_shared<FreePinFamily>(std::move(parsed_nodes));
  }
};

class FamilyWriter : public json::IFamilyWriter {
 public:
  explicit FamilyWriter(std::shared_ptr<FreePinFamily> family)
      : family_{std::move(family)} {}

  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto WriteToJson() const -> crude_json::value override { return {}; }

 private:
  std::shared_ptr<FreePinFamily> family_{};
};

auto CreateFamilyWriter(std::shared_ptr<FreePinFamily> family)
    -> std::unique_ptr<json::IFamilyWriter> {
  return std::make_unique<FamilyWriter>(std::move(family));
}

class FamilyDrawer : public coreui::IFamilyTraits {
 public:
  explicit FamilyDrawer(std::shared_ptr<FreePinFamily> family)
      : family_{std::move(family)} {}

  auto GetLabel() const -> std::string override { return "Free Pin"; }

  auto GetColor() const -> ImColor override { return {255, 255, 255}; }

  auto IsUserAccessible() -> bool { return false; }

 private:
  std::shared_ptr<FreePinFamily> family_{};
};

auto CreateFamilyDrawer(std::shared_ptr<FreePinFamily> family)
    -> std::unique_ptr<coreui::IFamilyTraits> {
  return std::make_unique<FamilyDrawer>(std::move(family));
}
}  // namespace

auto FreePinFamily::CreateParser() -> std::unique_ptr<json::IFamilyParser> {
  return std::make_unique<FamilyParser>();
}
}  // namespace esc::core