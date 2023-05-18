/**
 * PON Calculator https://github.com/qoala101/ponc
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "app_splitter_family_group.h"

#include <cmath>
#include <cstdint>
#include <memory>
#include <numeric>

#include "core_i_node.h"
#include "core_id_generator.h"
#include "coreui_empty_pin_traits.h"
#include "coreui_float_pin_traits.h"
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
#include "style_tailwind.h"
#include "style_utils.h"

namespace vh::ponc {
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

  ///
  auto GetDrop() const {
    switch (GetOutputPinIds().size()) {
      case 2:
        return -4.3F;
      case 4:
        return -7.4F;
      case 8:
        return -10.7F;
      case 16:
      default:
        return -13.9F;
    };
  }

  ///
  void SetInitialFlowValues(flow::NodeFlow& node_flow) const override {
    const auto drop = GetDrop();

    for (auto& [pin_id, flow] : node_flow.output_pin_flows) {
      flow = drop;
    }
  }

 private:
  ///
  cpp::SafeOwner safe_owner_{};
};

///
class HeaderUiTraits : public coreui::IHeaderTraits {
 public:
  ///
  explicit HeaderUiTraits(cpp::SafePtr<const Node> node)
      : node_{std::move(node)} {}

  ///
  auto GetColor() const -> ImColor override {
    const auto index = std::log2(node_->GetOutputPinIds().size());

    return style::GetGradient(
        style::Tailwind::GetColor(style::Tailwind::Color::kBlue,
                                  style::Tailwind::Shade::k800),
        style::Tailwind::GetColor(style::Tailwind::Color::kBlue,
                                  style::Tailwind::Shade::k600),
        1 - static_cast<float>(index) * 0.25F);
  }

 private:
  ///
  cpp::SafePtr<const Node> node_;
};

///
constexpr auto kLabel = "Splitter";

///
auto GetLabelWithOutputPins(int num_output_pins) {
  return std::string{kLabel} + " 1x" + std::to_string(num_output_pins);
}

///
class NodeUiTraits : public coreui::INodeTraits {
 public:
  ///
  explicit NodeUiTraits(cpp::SafePtr<const Node> node)
      : node_{std::move(node)} {}

  ///
  auto GetLabel() const -> std::string override {
    return GetLabelWithOutputPins(
        static_cast<int>(node_->GetOutputPinIds().size()));
  }

  ///
  auto CreateHeaderTraits() const
      -> std::optional<std::unique_ptr<coreui::IHeaderTraits>> override {
    return std::make_unique<HeaderUiTraits>(node_);
  }

  ///
  auto CreatePinTraits() const
      -> std::vector<std::unique_ptr<coreui::IPinTraits>> override {
    const auto& output_pins = node_->GetOutputPinIds();

    auto pin_traits = std::vector<std::unique_ptr<coreui::IPinTraits>>{};
    pin_traits.reserve(2 + output_pins.size());

    pin_traits.emplace_back(std::make_unique<coreui::FlowPinTraits>(
        core::INode::GetFirstPinOfKind(*node_, ne::PinKind::Input)));
    pin_traits.emplace_back(
        std::make_unique<coreui::FloatPinTraits>(node_->GetDrop()));

    for (const auto pin_id : node_->GetOutputPinIds()) {
      pin_traits.emplace_back(std::make_unique<coreui::FlowPinTraits>(pin_id));
    }

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
auto CreateFamilyNodeParser() -> std::unique_ptr<json::INodeParser>;
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
  Family(core::FamilyId id, int num_output_pins)
      : IFamily{id}, num_output_pins_{num_output_pins} {}

  ///
  auto CreateNode(core::IdGenerator& id_generator) const
      -> std::unique_ptr<core::INode> override {
    return std::make_unique<Node>(core::INode::ConstructorArgs{
        .id = id_generator.Generate<ne::NodeId>(),
        .family_id = GetId(),
        .input_pin_id = id_generator.Generate<ne::PinId>(),
        .output_pin_ids = id_generator.GenerateN<ne::PinId>(num_output_pins_)});
  }

  ///
  auto CreateNodeParser() const -> std::unique_ptr<json::INodeParser> override {
    return CreateFamilyNodeParser();
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
  auto GetNumOutputPins() const { return num_output_pins_; }

 private:
  ///
  int num_output_pins_{};
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
auto CreateFamilyNodeParser() -> std::unique_ptr<json::INodeParser> {
  return std::make_unique<NodeParser>();
}

///
constexpr auto kTypeName = "Splitter";

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
        parsed_id, json["num_output_pins"].get<crude_json::number>());
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
    json["num_output_pins"] =
        static_cast<crude_json::number>(family_->GetNumOutputPins());
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
  explicit FamilyUiTraits(cpp::SafePtr<const Family> family)
      : family_{std::move(family)} {}

  ///
  auto GetLabel() const -> std::string override {
    return GetLabelWithOutputPins(family_->GetNumOutputPins());
  }

  ///
  auto GetGroupLabel() const -> std::string override { return kLabel; }

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
auto SplitterFamilyGroup::CreateFamilies(core::IdGenerator& id_generator) const
    -> std::vector<std::unique_ptr<core::IFamily>> {
  auto powers = std::vector<int>(4);
  std::iota(powers.begin(), powers.end(), 1);

  auto families = std::vector<std::unique_ptr<core::IFamily>>{};
  families.reserve(powers.size());

  for (const auto power : powers) {
    const auto num_output_pins = std::pow(2, power);

    families.emplace_back(std::make_unique<Family>(
        id_generator.Generate<core::FamilyId>(), num_output_pins));
  }

  return families;
}

///
auto SplitterFamilyGroup::CreateFamilyParser() const
    -> std::unique_ptr<json::IFamilyParser> {
  return std::make_unique<FamilyParser>();
}
}  // namespace vh::ponc