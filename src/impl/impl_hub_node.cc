#include "impl_hub_node.h"

#include <bits/ranges_algo.h>

#include <memory>
#include <string>

#include "core_i_node.h"
#include "core_id_generator.h"
#include "core_link.h"
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

constexpr auto kTypeName = "HubNode";

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

  auto GetInitialFlow [[nodiscard]] () const -> core::Flow override {
    return {};
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

class InputPinDrawer : public draw::IPinDrawer {
 public:
  explicit InputPinDrawer(int num_connected_links)
      : num_connected_links_{num_connected_links} {}

  auto GetLabel [[nodiscard]] () const -> std::string {
    return std::to_string(num_connected_links_);
  }

  auto GetKind [[nodiscard]] () const -> ne::PinKind override {
    return ne::PinKind::Input;
  }

 private:
  int num_connected_links_{};
};

class OutputPinDrawer : public draw::IPinDrawer {
 public:
  explicit OutputPinDrawer(int num_connected_links)
      : num_connected_links_{num_connected_links} {}

  auto GetLabel [[nodiscard]] () const -> std::string {
    return std::to_string(num_connected_links_);
  }

  auto GetKind [[nodiscard]] () const -> ne::PinKind override {
    return ne::PinKind::Output;
  }

 private:
  int num_connected_links_{};
};

class NodeDrawer : public draw::INodeDrawer {
 public:
  explicit NodeDrawer(std::shared_ptr<Node> node, const State& state) : node_{std::move(node)} {
    const auto& pin_ids = node_->GetPinIds();
    const auto& links = state.app_.GetDiagram().GetLinks();

    num_connected_input_links_ = static_cast<int>(std::ranges::count_if(
        links, [node_pin = pin_ids[0]](const core::Link& link) {
          return link.end_pin_id == node_pin;
        }));
    num_connected_output_links_ = static_cast<int>(std::ranges::count_if(
        links, [node_pin = pin_ids[1]](const core::Link& link) {
          return link.start_pin_id == node_pin;
        }));
  }

  auto GetLabel() const -> std::string override {
    return HubNode::CreateFamily()->CreateDrawer()->GetLabel();
  }

  auto GetColor() const -> ImColor override {
    return HubNode::CreateFamily()->CreateDrawer()->GetColor();
  }

  auto CreatePinDrawer(ne::PinId pin_id) const
      -> std::unique_ptr<draw::IPinDrawer> override {
    const auto pin_index = node_->GetPinIndex(pin_id);

    if (pin_index == 0) {
      return std::make_unique<InputPinDrawer>(num_connected_input_links_);
    }

    if (pin_index == 1) {
      return std::make_unique<OutputPinDrawer>(num_connected_output_links_);
    }

    cpp::Expects(false);
  }

 private:
  std::shared_ptr<Node> node_{};
  int num_connected_input_links_{};
  int num_connected_output_links_{};
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
                                  id_generator.GetNextN<ne::PinId>(2));
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

  auto GetLabel() const -> std::string override { return "Hub"; }

  auto GetColor() const -> ImColor override { return {255, 255, 255}; }

 private:
  std::shared_ptr<Family> family_{};
};

auto CreateFamilyDrawer(std::shared_ptr<Family> family)
    -> std::unique_ptr<draw::IFamilyDrawer> {
  return std::make_unique<FamilyDrawer>(std::move(family));
}
}  // namespace

auto HubNode::CreateFamily() -> std::shared_ptr<core::IFamily> {
  return std::make_unique<Family>();
}

auto HubNode::CreateFamilyParser() -> std::unique_ptr<json::IFamilyParser> {
  return std::make_unique<FamilyParser>();
}
}  // namespace esc::impl