/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "core_free_pin_family_group.h"

#include <crude_json.h>
#include <imgui.h>
#include <imgui_node_editor.h>

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "core_i_family.h"
#include "core_i_node.h"
#include "core_id_generator.h"
#include "coreui_flow_pin_traits.h"
#include "coreui_i_family_traits.h"
#include "coreui_i_node_traits.h"
#include "coreui_i_pin_traits.h"
#include "cpp_safe_ptr.h"
#include "json_i_family_writer.h"
#include "json_i_node_parser.h"
#include "style_default_colors.h"

namespace vh::ponc::core {
namespace {
///
class Node;

///
auto CreateNodeUiTraits(cpp::SafePtr<const Node> node)
    -> std::unique_ptr<coreui::INodeTraits>;

///
class Node : public core::INode {
 public:
  ///
  explicit Node(const ConstructorArgs& args) : INode{args} {}

  ///
  auto CreateUiTraits() -> std::unique_ptr<coreui::INodeTraits> override {
    return CreateNodeUiTraits(safe_owner_.MakeSafe(this));
  }

 private:
  ///
  cpp::SafeOwner safe_owner_{};
};

///
class NodeParser : public json::INodeParser {
 private:
  ///
  auto ParseFromJson(const core::INode::ConstructorArgs& parsed_args,
                     const crude_json::value& /*unused*/) const
      -> std::unique_ptr<core::INode> override {
    return std::make_unique<Node>(parsed_args);
  }
};

///
class PinTraits : public coreui::FlowPinTraits {
 public:
  ///
  using FlowPinTraits::FlowPinTraits;

  ///
  auto GetValue() const -> coreui::PinValueVariant override { return {}; }

  ///
  auto GetLabel() const -> std::optional<coreui::PinLabel> override {
    return coreui::PinLabel{.color = style::DefaultColors::kWhite};
  }
};

///
auto GetLabelWithPinKind(ne::PinKind pin_kind) {
  return (pin_kind == ne::PinKind::Input) ? "Free Input Pin"
                                          : "Free Output Pin";
}

///
class NodeUiTraits : public coreui::INodeTraits {
 public:
  ///
  explicit NodeUiTraits(cpp::SafePtr<const Node> node)
      : node_{std::move(node)} {}

  ///
  auto GetLabel() const -> std::string override {
    return GetLabelWithPinKind(node_->GetInputPinId().has_value()
                                   ? ne::PinKind::Input
                                   : ne::PinKind::Output);
  }

  ///
  auto CreatePinTraits() const
      -> std::vector<std::unique_ptr<coreui::IPinTraits>> override {
    auto pin_traits = std::vector<std::unique_ptr<coreui::IPinTraits>>{};
    pin_traits.reserve(1);

    if (const auto& input_pin = node_->GetInputPinId()) {
      pin_traits.emplace_back(std::make_unique<PinTraits>(*input_pin));
      return pin_traits;
    }

    pin_traits.emplace_back(std::make_unique<PinTraits>(
        core::INode::GetFirstPinOfKind(*node_, ne::PinKind::Output)));

    return pin_traits;
  }

 private:
  ///
  cpp::SafePtr<const Node> node_;
};

///
auto CreateNodeUiTraits(cpp::SafePtr<const Node> node)
    -> std::unique_ptr<coreui::INodeTraits> {
  return std::make_unique<NodeUiTraits>(std::move(node));
}

///
class Family;

///
auto CreateFamilyWriter(cpp::SafePtr<const Family> family)
    -> std::unique_ptr<json::IFamilyWriter>;
///
auto CreateFamilyUiTraits(cpp::SafePtr<const Family> family)
    -> std::unique_ptr<coreui::IFamilyTraits>;

///
class Family : public core::IFamily {
 public:
  ///
  explicit Family(core::FamilyId id, ne::PinKind pin_kind)
      : IFamily{id}, pin_kind_{pin_kind} {}

  ///
  auto GetType() const -> std::optional<FamilyType> override {
    return FamilyType::kFreePin;
  }

  ///
  auto CreateNode(core::IdGenerator& id_generator) const
      -> std::unique_ptr<core::INode> override {
    auto args = core::INode::ConstructorArgs{
        .id = id_generator.Generate<ne::NodeId>(), .family_id = GetId()};

    if (pin_kind_ == ne::PinKind::Input) {
      args.input_pin_id = id_generator.Generate<ne::PinId>();
      return std::make_unique<Node>(args);
    }

    args.output_pin_ids = id_generator.GenerateN<ne::PinId>(1);
    return std::make_unique<Node>(args);
  }

  ///
  auto CreateNodeParser() const -> std::unique_ptr<json::INodeParser> override {
    return std::make_unique<NodeParser>();
  }

  ///
  auto CreateWriter() const -> std::unique_ptr<json::IFamilyWriter> override {
    return CreateFamilyWriter(safe_owner_.MakeSafe(this));
  }

  ///
  auto CreateUiTraits() const
      -> std::unique_ptr<coreui::IFamilyTraits> override {
    return CreateFamilyUiTraits(safe_owner_.MakeSafe(this));
  }

  ///
  auto GetPinKind() const { return pin_kind_; }

 private:
  ///
  ne::PinKind pin_kind_{};
  ///
  cpp::SafeOwner safe_owner_{};
};

///
constexpr auto kTypeName = "FreePin";

///
class FamilyParser : public json::IFamilyParser {
 public:
  ///
  auto GetTypeName() const -> std::string override { return kTypeName; }

  ///
  auto ParseFromJson(core::FamilyId parsed_id,
                     const crude_json::value& json) const
      -> std::unique_ptr<core::IFamily> override {
    return std::make_unique<Family>(
        parsed_id,
        static_cast<ne::PinKind>(json["pin_kind"].get<crude_json::number>()));
  }
};

///
class FamilyWriter : public json::IFamilyWriter {
 public:
  ///
  explicit FamilyWriter(cpp::SafePtr<const Family> family)
      : family_{std::move(family)} {}

 private:
  ///
  auto GetTypeName() const -> std::string override { return kTypeName; }

  ///
  auto WriteToJson() const -> crude_json::value override {
    auto json = crude_json::value{};
    json["pin_kind"] = static_cast<crude_json::number>(family_->GetPinKind());
    return json;
  }

  ///
  cpp::SafePtr<const Family> family_;
};

///
auto CreateFamilyWriter(cpp::SafePtr<const Family> family)
    -> std::unique_ptr<json::IFamilyWriter> {
  return std::make_unique<FamilyWriter>(std::move(family));
}

///
class FamilyUiTraits : public coreui::IFamilyTraits {
 public:
  ///
  explicit FamilyUiTraits(cpp::SafePtr<const Family> family)
      : family_{std::move(family)} {}

  ///
  auto GetLabel() const -> std::string override {
    return GetLabelWithPinKind(family_->GetPinKind());
  }

  ///
  auto GetGroupLabel() const -> std::string override { return "Free Pin"; }

 private:
  ///
  cpp::SafePtr<const Family> family_;
};

///
auto CreateFamilyUiTraits(cpp::SafePtr<const Family> family)
    -> std::unique_ptr<coreui::IFamilyTraits> {
  return std::make_unique<FamilyUiTraits>(std::move(family));
}
}  // namespace

///
auto FreePinFamilyGroup::CreateFamilies(core::IdGenerator& id_generator) const
    -> std::vector<std::unique_ptr<core::IFamily>> {
  auto families = std::vector<std::unique_ptr<core::IFamily>>{};
  families.reserve(2);

  families.emplace_back(std::make_unique<Family>(
      id_generator.Generate<core::FamilyId>(), ne::PinKind::Input));
  families.emplace_back(std::make_unique<Family>(
      id_generator.Generate<core::FamilyId>(), ne::PinKind::Output));

  return families;
}

///
auto FreePinFamilyGroup::CreateFamilyParser() const
    -> std::unique_ptr<json::IFamilyParser> {
  return std::make_unique<FamilyParser>();
}
}  // namespace vh::ponc::core