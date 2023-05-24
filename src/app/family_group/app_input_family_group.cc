/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "app_input_family_group.h"

#include <crude_json.h>
#include <imgui.h>
#include <imgui_node_editor.h>

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "core_i_node.h"
#include "core_id_generator.h"
#include "coreui_i_family_traits.h"
#include "coreui_i_header_traits.h"
#include "coreui_i_node_traits.h"
#include "coreui_i_pin_traits.h"
#include "cpp_safe_ptr.h"
#include "flow_node_flow.h"
#include "json_i_family_writer.h"
#include "json_i_node_parser.h"
#include "json_i_node_writer.h"
#include "style_tailwind.h"

namespace vh::ponc {
namespace {
///
class Node;

///
auto CreateNodeWriter(cpp::SafePtr<const Node> node)
    -> std::unique_ptr<json::INodeWriter>;
///
auto CreateNodeUiTraits(cpp::SafePtr<Node> node)
    -> std::unique_ptr<coreui::INodeTraits>;

///
class Node : public core::INode {
 public:
  ///
  explicit Node(const ConstructorArgs& args, float value = 6)
      : INode{args}, value_{value} {}

  ///
  auto CreateWriter() const -> std::unique_ptr<json::INodeWriter> override {
    return CreateNodeWriter(safe_owner_.MakeSafe(this));
  }

  ///
  auto CreateUiTraits() -> std::unique_ptr<coreui::INodeTraits> override {
    return CreateNodeUiTraits(safe_owner_.MakeSafe(this));
  }

  ///
  void SetInitialFlowValues(flow::NodeFlow& node_flow) const override {
    node_flow.output_pin_flows.at(
        core::INode::GetFirstPinOfKind(*this, ne::PinKind::Output).Get()) =
        value_;
  }

  ///
  auto GetValue() const { return value_; }

  ///
  auto GetValue() -> auto& { return value_; }

 private:
  ///
  float value_{};
  ///
  cpp::SafeOwner safe_owner_{};
};

///
class NodeParser : public json::INodeParser {
 private:
  ///
  auto ParseFromJson(const core::INode::ConstructorArgs& parsed_args,
                     const crude_json::value& json) const
      -> std::unique_ptr<core::INode> override {
    return std::make_unique<Node>(parsed_args,
                                  json["value"].get<crude_json::number>());
  }
};

///
class NodeWriter : public json::INodeWriter {
 public:
  ///
  explicit NodeWriter(cpp::SafePtr<const Node> node) : node_{std::move(node)} {}

 private:
  ///
  auto WriteToJson() const -> crude_json::value override {
    auto json = crude_json::value{};
    json["value"] = node_->GetValue();
    return json;
  }

  ///
  cpp::SafePtr<const Node> node_;
};

///
auto CreateNodeWriter(cpp::SafePtr<const Node> node)
    -> std::unique_ptr<json::INodeWriter> {
  return std::make_unique<NodeWriter>(std::move(node));
}

///
class HeaderUiTraits : public coreui::IHeaderTraits {
 public:
  ///
  auto GetColor() const -> ImColor override {
    return style::Tailwind::GetColor(style::Tailwind::Color::kRose,
                                     style::Tailwind::Shade::k700);
  }
};

///
class ValuePinTraits : public coreui::IPinTraits {
 public:
  ///
  explicit ValuePinTraits(cpp::SafePtr<Node> node) : node_{std::move(node)} {}

  ///
  auto GetPin() const -> std::variant<ne::PinId, ne::PinKind> override {
    return core::INode::GetFirstPinOfKind(*node_, ne::PinKind::Output);
  }

  ///
  auto GetValue() const -> coreui::PinValueVariant override {
    return &node_->GetValue();
  }

 private:
  ///
  cpp::SafePtr<Node> node_;
};

///
constexpr auto kLabel = "Input";

///
class NodeUiTraits : public coreui::INodeTraits {
 public:
  ///
  explicit NodeUiTraits(cpp::SafePtr<Node> node) : node_{std::move(node)} {}

  ///
  auto GetLabel() const -> std::string override { return kLabel; }

  ///
  auto CreateHeaderTraits() const
      -> std::optional<std::unique_ptr<coreui::IHeaderTraits>> override {
    return std::make_unique<HeaderUiTraits>();
  }

  ///
  auto CreatePinTraits() const
      -> std::vector<std::unique_ptr<coreui::IPinTraits>> override {
    auto pin_traits = std::vector<std::unique_ptr<coreui::IPinTraits>>{};
    pin_traits.reserve(1);

    pin_traits.emplace_back(std::make_unique<ValuePinTraits>(node_));
    return pin_traits;
  }

 private:
  ///
  cpp::SafePtr<Node> node_;
};

///
auto CreateNodeUiTraits(cpp::SafePtr<Node> node)
    -> std::unique_ptr<coreui::INodeTraits> {
  return std::make_unique<NodeUiTraits>(std::move(node));
}

///
auto CreateFamilyWriter() -> std::unique_ptr<json::IFamilyWriter>;
///
auto CreateFamilyUiTraits() -> std::unique_ptr<coreui::IFamilyTraits>;

///
class Family : public core::IFamily {
 public:
  ///
  explicit Family(core::FamilyId id) : IFamily{id} {}

  ///
  auto CreateNode(core::IdGenerator& id_generator) const
      -> std::unique_ptr<core::INode> override {
    return std::make_unique<Node>(core::INode::ConstructorArgs{
        .id = id_generator.Generate<ne::NodeId>(),
        .family_id = GetId(),
        .output_pin_ids = id_generator.GenerateN<ne::PinId>(1)});
  }

  ///
  auto CreateNodeParser() const -> std::unique_ptr<json::INodeParser> override {
    return std::make_unique<NodeParser>();
  }

  ///
  auto CreateWriter() const -> std::unique_ptr<json::IFamilyWriter> override {
    return CreateFamilyWriter();
  }

  ///
  auto CreateUiTraits() const
      -> std::unique_ptr<coreui::IFamilyTraits> override {
    return CreateFamilyUiTraits();
  }
};

///
constexpr auto kTypeName = "Input";

///
class FamilyParser : public json::IFamilyParser {
 public:
  ///
  auto GetTypeName() const -> std::string override { return kTypeName; }

  ///
  auto ParseFromJson(core::FamilyId parsed_id,
                     const crude_json::value& /*unused*/) const
      -> std::unique_ptr<core::IFamily> override {
    return std::make_unique<Family>(parsed_id);
  }
};

///
class FamilyWriter : public json::IFamilyWriter {
 private:
  ///
  auto GetTypeName() const -> std::string override { return kTypeName; }
};

///
auto CreateFamilyWriter() -> std::unique_ptr<json::IFamilyWriter> {
  return std::make_unique<FamilyWriter>();
}

///
class FamilyUiTraits : public coreui::IFamilyTraits {
 public:
  ///
  auto GetLabel() const -> std::string override { return kLabel; }
};

///
auto CreateFamilyUiTraits() -> std::unique_ptr<coreui::IFamilyTraits> {
  return std::make_unique<FamilyUiTraits>();
}
}  // namespace

///
auto InputFamilyGroup::CreateFamilies(core::IdGenerator& id_generator) const
    -> std::vector<std::unique_ptr<core::IFamily>> {
  auto families = std::vector<std::unique_ptr<core::IFamily>>{};
  families.reserve(1);

  families.emplace_back(
      std::make_unique<Family>(id_generator.Generate<core::FamilyId>()));

  return families;
}

///
auto InputFamilyGroup::CreateFamilyParser() const
    -> std::unique_ptr<json::IFamilyParser> {
  return std::make_unique<FamilyParser>();
}
}  // namespace vh::ponc