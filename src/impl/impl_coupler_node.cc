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
auto CreateNodeDrawer(std::shared_ptr<Node> node)
    -> std::unique_ptr<draw::INodeDrawer>;
auto CreateFamilyWriter(std::shared_ptr<Family> family)
    -> std::unique_ptr<json::IFamilyWriter>;
auto CreateFamilyDrawer(std::shared_ptr<Family> family)
    -> std::unique_ptr<draw::IFamilyDrawer>;

// NOLINTNEXTLINE(*-multiple-inheritance)
class Node : public core::INode, public std::enable_shared_from_this<Node> {
 public:
  Node(ne::NodeId id, std::vector<ne::PinId> pin_ids, int percentage_index = 0)
      : INode{id, std::move(pin_ids)}, percentage_index_{percentage_index} {}

  auto CreateWriter() -> std::unique_ptr<json::INodeWriter> override {
    return CreateNodeWriter(shared_from_this());
  }

  auto CreateDrawer() -> std::unique_ptr<draw::INodeDrawer> override {
    return CreateNodeDrawer(shared_from_this());
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
  explicit SmallDropPinDrawer(int percentage_index)
      : percentage_index_{percentage_index}, drop_{[percentage_index]() {
          switch (percentage_index) {
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
        }()} {}

  auto GetLabel [[nodiscard]] () const -> std::string override {
    return std::to_string((percentage_index_ + 1) * 5) + "%";
  }

  auto GetKind [[nodiscard]] () const -> ne::PinKind override {
    return ne::PinKind::Input;
  }

  auto GetFloat [[nodiscard]] () -> float* override { return &drop_; }

  auto IsEditable [[nodiscard]] () const -> bool override { return false; }

  auto IsConnectable [[nodiscard]] () const -> bool override { return false; }

 private:
  int percentage_index_{};
  float drop_{};
};

class BigDropPinDrawer : public draw::IPinDrawer {
 public:
  explicit BigDropPinDrawer(int percentage_index)
      : percentage_index_{percentage_index}, drop_{[percentage_index]() {
          switch (percentage_index) {
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
        }()} {}

  auto GetLabel [[nodiscard]] () const -> std::string override {
    return std::to_string(100 - (percentage_index_ + 1) * 5) + "%";
  }

  auto GetKind [[nodiscard]] () const -> ne::PinKind override {
    return ne::PinKind::Input;
  }

  auto GetFloat [[nodiscard]] () -> float* override { return &drop_; }

  auto IsEditable [[nodiscard]] () const -> bool override { return false; }

  auto IsConnectable [[nodiscard]] () const -> bool override { return false; }

 private:
  int percentage_index_{};
  float drop_{};
};

class NodeDrawer : public draw::INodeDrawer {
 public:
  explicit NodeDrawer(std::shared_ptr<Node> node) : node_{std::move(node)} {}

  auto GetLabel() const -> std::string override {
    return CouplerNode::CreateFamily()->CreateDrawer()->GetLabel();
  }

  auto GetColor() const -> ImColor override {
    return CouplerNode::CreateFamily()->CreateDrawer()->GetColor();
  }

  auto CreatePinDrawer(ne::PinId pin_id) const
      -> std::unique_ptr<draw::IPinDrawer> override {
    const auto pin_index = node_->GetPinIndex(pin_id);

    if (pin_index == 0) {
      return std::make_unique<draw::FlowInputPinDrawer>();
    }

    if (pin_index == 1) {
      return std::make_unique<SmallDropPinDrawer>(node_->percentage_index_);
    }

    if (pin_index == 2) {
      return std::make_unique<BigDropPinDrawer>(node_->percentage_index_);
    }

    if (pin_index == 3) {
      return std::make_unique<draw::EmptyPinDrawer>(ne::PinKind::Output);
    }

    return std::make_unique<draw::FlowOutputPinDrawer>();

    cpp::Expects(false);
  }

 private:
  std::shared_ptr<Node> node_{};
};

auto CreateNodeDrawer(std::shared_ptr<Node> node)
    -> std::unique_ptr<draw::INodeDrawer> {
  return std::make_unique<NodeDrawer>(std::move(node));
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
                                  id_generator.GetNextN<ne::PinId>(6));
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

  auto GetLabel() const -> std::string override { return "Coupler"; }

  auto GetColor() const -> ImColor override { return {255, 0, 255}; }

 private:
  std::shared_ptr<Family> family_{};
};

auto CreateFamilyDrawer(std::shared_ptr<Family> family)
    -> std::unique_ptr<draw::IFamilyDrawer> {
  return std::make_unique<FamilyDrawer>(std::move(family));
}
}  // namespace

auto CouplerNode::CreateFamily() -> std::shared_ptr<core::IFamily> {
  return std::make_unique<Family>();
}

auto CouplerNode::CreateFamilyParser() -> std::unique_ptr<json::IFamilyParser> {
  return std::make_unique<FamilyParser>();
}
}  // namespace esc::impl