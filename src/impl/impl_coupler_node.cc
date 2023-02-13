#include "impl_coupler_node.h"

#include <memory>

#include "core_i_node.h"
#include "core_id_generator.h"
#include "cpp_assert.h"
#include "crude_json.h"
#include "draw_empty_pin_drawer.h"
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

constexpr auto kTypeName = "CouplerNode";

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
  Node(ne::NodeId id, std::vector<ne::PinId> pin_ids, int percentage_index = {})
      : INode{id, std::move(pin_ids)}, percentage_index_{percentage_index} {}

  auto CreateWriter() -> std::unique_ptr<json::INodeWriter> override {
    return CreateNodeWriter(shared_from_this());
  }

  auto CreateDrawer(const State& state)
      -> std::unique_ptr<draw::INodeDrawer> override {
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

  auto GetInitialFlow [[nodiscard]] () const -> core::Flow override {
    const auto& pin_ids = GetPinIds();

    return {.input_pin_flow = std::pair{pin_ids[0].Get(), float{}},
            .output_pin_flows = {{pin_ids[4].Get(), GetSmallDrop()},
                                 {pin_ids[5].Get(), GetBigDrop()}}};
  }

  int percentage_index_{};
};

class NodeParser : public json::INodeParser {
 private:
  auto ParseFromJson(ne::NodeId parsed_node_id,
                     std::vector<ne::PinId> parsed_pin_ids,
                     const crude_json::value& json) const
      -> std::shared_ptr<core::INode> override {
    return std::make_shared<Node>(
        parsed_node_id, std::move(parsed_pin_ids),
        json["percentage_index"].get<crude_json::number>());
  }
};

class NodeWriter : public json::INodeWriter {
 public:
  explicit NodeWriter(std::shared_ptr<Node> node) : node_{std::move(node)} {}

 private:
  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto WriteToJson() const -> crude_json::value override {
    auto json = crude_json::value{};
    json["percentage_index"] =
        static_cast<crude_json::number>(node_->percentage_index_);
    return json;
  }

  std::shared_ptr<Node> node_{};
};

auto CreateNodeWriter(std::shared_ptr<Node> node)
    -> std::unique_ptr<json::INodeWriter> {
  return std::make_unique<NodeWriter>(std::move(node));
}

class SmallDropPinDrawer : public draw::IPinDrawer {
 public:
  explicit SmallDropPinDrawer(float drop) : drop_{drop} {}

  auto GetKind [[nodiscard]] () const -> ne::PinKind override {
    return ne::PinKind::Input;
  }

  auto GetFloat [[nodiscard]] () -> float* override { return &drop_; }

  auto IsEditable [[nodiscard]] () const -> bool override { return false; }

 private:
  float drop_{};
};

class BigDropPinDrawer : public draw::IPinDrawer {
 public:
  explicit BigDropPinDrawer(float drop) : drop_{drop} {}

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
    return CouplerNode::CreateFamily(node_->percentage_index_)
        ->CreateDrawer()
        ->GetLabel();
  }

  auto GetColor() const -> ImColor override {
    return CouplerNode::CreateFamily(node_->percentage_index_)
        ->CreateDrawer()
        ->GetColor();
  }

  auto CreatePinDrawer(ne::PinId pin_id) const
      -> std::unique_ptr<draw::IPinDrawer> override {
    const auto pin_index = node_->GetPinIndex(pin_id);

    if (pin_index == 0) {
      return std::make_unique<draw::FlowInputPinDrawer>(
          flow_pin_values_.input_pin_flow->second);
    }

    if (pin_index == 1) {
      return std::make_unique<SmallDropPinDrawer>(node_->GetSmallDrop());
    }

    if (pin_index == 2) {
      return std::make_unique<BigDropPinDrawer>(node_->GetBigDrop());
    }

    if (pin_index == 3) {
      return std::make_unique<draw::EmptyPinDrawer>(ne::PinKind::Output);
    }

    return std::make_unique<draw::FlowOutputPinDrawer>(
        flow_pin_values_.output_pin_flows.at(pin_id.Get()));
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
  explicit Family(int percentage_index,
                  std::vector<std::shared_ptr<core::INode>> nodes = {})
      : IFamily{std::move(nodes)}, percentage_index_{percentage_index} {}

  auto CreateNode(core::IdGenerator& id_generator)
      -> std::shared_ptr<core::INode> override {
    return std::make_shared<Node>(id_generator.GetNext<ne::NodeId>(),
                                  id_generator.GetNextN<ne::PinId>(6),
                                  percentage_index_);
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

class FamilyDrawer : public draw::IFamilyDrawer {
 public:
  explicit FamilyDrawer(std::shared_ptr<Family> family)
      : family_{std::move(family)} {}

  auto GetLabel() const -> std::string override {
    const auto percentage = (family_->GetPercentageIndex() + 1) * 5;
    return "Coupler " + std::to_string(percentage) + "%-" +
           std::to_string(100 - percentage) + "%";
  }

  auto GetColor() const -> ImColor override { return {255, 0, 255}; }

 private:
  std::shared_ptr<Family> family_{};
};

auto CreateFamilyDrawer(std::shared_ptr<Family> family)
    -> std::unique_ptr<draw::IFamilyDrawer> {
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
}  // namespace esc::impl