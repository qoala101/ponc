/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "app_coupler_family_group.h"

#include <cstdint>
#include <memory>

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
#include "cpp_safe_ptr.h"
#include "crude_json.h"
#include "imgui_node_editor.h"
#include "json_i_family_writer.h"
#include "json_i_node_parser.h"
#include "json_i_node_writer.h"
#include "style_tailwind.h"
#include "style_utils.h"

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
struct Drop {
  ///
  float small{};
  ///
  float big{};
};

///
constexpr auto kDrops = std::array{
    Drop{-0.4, -13.8}, Drop{-0.7, -10.6}, Drop{-0.95, -8.8}, Drop{-1.2, -7.5},
    Drop{-1.55, -6.5}, Drop{-1.85, -5.7}, Drop{-2.2, -5},    Drop{-2.6, -4.4},
    Drop{-3, -3.9},    Drop{-3.4, -3.4},
};

///
class Node : public core::INode {
 public:
  ///
  Node(const ConstructorArgs& args, int percentage_index,
       bool reverse_order = false)
      : INode{args},
        percentage_index_{percentage_index},
        reverse_order_{reverse_order} {}

  ///
  auto CreateWriter() const -> std::unique_ptr<json::INodeWriter> override {
    return CreateNodeWriter(safe_owner_.MakeSafe(this));
  }

  ///
  auto CreateUiTraits() -> std::unique_ptr<coreui::INodeTraits> override {
    return CreateNodeUiTraits(safe_owner_.MakeSafe(this));
  }

  ///
  auto GetFirstDrop() const {
    return reverse_order_ ? GetBigDrop() : GetSmallDrop();
  }

  ///
  auto GetSecondDrop() const {
    return reverse_order_ ? GetSmallDrop() : GetBigDrop();
  }

  ///
  void SetInitialFlowValues(flow::NodeFlow& node_flow) const override {
    const auto& output_pins = GetOutputPinIds();
    Expects(output_pins.size() > 1);

    node_flow.output_pin_flows.at(output_pins[0].Get()) = GetFirstDrop();
    node_flow.output_pin_flows.at(output_pins[1].Get()) = GetSecondDrop();
  }

  ///
  auto GetPercentageIndex() const { return percentage_index_; }

  ///
  auto IsOrderReversed() const { return reverse_order_; }

  ///
  void ReverseOrder() { reverse_order_ = !reverse_order_; }

 private:
  ///
  auto GetSmallDrop() const -> float {
    Expects(percentage_index_ >= 0);
    Expects(percentage_index_ < static_cast<int>(kDrops.size()));

    // NOLINTNEXTLINE(*-constant-array-index)
    return kDrops[percentage_index_].small;
  }

  ///
  auto GetBigDrop() const -> float {
    Expects(percentage_index_ >= 0);
    Expects(percentage_index_ < static_cast<int>(kDrops.size()));

    // NOLINTNEXTLINE(*-constant-array-index)
    return kDrops[percentage_index_].big;
  }

  ///
  int percentage_index_{};
  ///
  bool reverse_order_{};
  ///
  cpp::SafeOwner safe_owner_{};
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

    if (node_->IsOrderReversed()) {
      json["reverse_order"] = true;
    }

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
  explicit HeaderUiTraits(cpp::SafePtr<const Node> node)
      : node_{std::move(node)} {}

  ///
  auto GetColor() const -> ImColor override {
    return style::GetGradient(
        style::Tailwind::GetColor(style::Tailwind::Color::kPurple,
                                  style::Tailwind::Shade::k800),
        style::Tailwind::GetColor(style::Tailwind::Color::kPurple,
                                  style::Tailwind::Shade::k600),
        1 - static_cast<float>(node_->GetPercentageIndex()) * 0.1F);
  }

 private:
  ///
  cpp::SafePtr<const Node> node_;
};

///
constexpr auto kLabel = "Coupler";

///
auto GetLabelWithPercentage(int percentage_index) {
  const auto percentage = (percentage_index + 1) * 5;

  return std::string{kLabel} + " " + std::to_string(percentage) + "%-" +
         std::to_string(100 - percentage) + "%";
}

///
class NodeUiTraits : public coreui::INodeTraits {
 public:
  ///
  explicit NodeUiTraits(cpp::SafePtr<Node> node) : node_{std::move(node)} {}

  ///
  auto GetLabel() const -> std::string override {
    auto label = GetLabelWithPercentage(node_->GetPercentageIndex());

    if (node_->IsOrderReversed()) {
      label += " *";
    }

    return label;
  }

  ///
  auto CreateHeaderTraits() const
      -> std::optional<std::unique_ptr<coreui::IHeaderTraits>> override {
    return std::make_unique<HeaderUiTraits>(node_);
  }

  ///
  auto CreatePinTraits() const
      -> std::vector<std::unique_ptr<coreui::IPinTraits>> override {
    auto pin_traits = std::vector<std::unique_ptr<coreui::IPinTraits>>{};
    pin_traits.reserve(6);

    pin_traits.emplace_back(std::make_unique<coreui::FlowPinTraits>(
        core::INode::GetFirstPinOfKind(*node_, ne::PinKind::Input)));
    pin_traits.emplace_back(
        std::make_unique<coreui::FloatPinTraits>(node_->GetFirstDrop()));
    pin_traits.emplace_back(
        std::make_unique<coreui::FloatPinTraits>(node_->GetSecondDrop()));
    pin_traits.emplace_back(
        std::make_unique<coreui::EmptyPinTraits>(ne::PinKind::Output));

    const auto& output_pins = node_->GetOutputPinIds();
    Expects(output_pins.size() > 1);

    pin_traits.emplace_back(
        std::make_unique<coreui::FlowPinTraits>(output_pins[0]));
    pin_traits.emplace_back(
        std::make_unique<coreui::FlowPinTraits>(output_pins[1]));

    return pin_traits;
  }

  ///
  auto GetActionNames() const -> std::vector<std::string> override {
    if (const auto same_drops =
            node_->GetPercentageIndex() == (kDrops.size() - 1)) {
      return {};
    }

    return {"Reverse Order"};
  }

  ///
  void ExecuteAction(std::string_view action_name) override {
    if (action_name == "Reverse Order") {
      node_->ReverseOrder();
    }
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
class Family;

///
auto CreateFamilyNodeParser(cpp::SafePtr<const Family> family)
    -> std::unique_ptr<json::INodeParser>;
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
  Family(core::FamilyId id, int percentage_index)
      : IFamily{id}, percentage_index_{percentage_index} {}

  ///
  auto CreateNode(core::IdGenerator& id_generator) const
      -> std::unique_ptr<core::INode> override {
    return std::make_unique<Node>(
        core::INode::ConstructorArgs{
            .id = id_generator.Generate<ne::NodeId>(),
            .family_id = GetId(),
            .input_pin_id = id_generator.Generate<ne::PinId>(),
            .output_pin_ids = id_generator.GenerateN<ne::PinId>(2)},
        percentage_index_);
  }

  ///
  auto CreateNodeParser() const -> std::unique_ptr<json::INodeParser> override {
    return CreateFamilyNodeParser(safe_owner_.MakeSafe(this));
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
  auto GetPercentageIndex() const { return percentage_index_; }

 private:
  ///
  int percentage_index_{};
  ///
  cpp::SafeOwner safe_owner_{};
};

///
class NodeParser : public json::INodeParser {
 public:
  ///
  explicit NodeParser(cpp::SafePtr<const Family> family)
      : family_{std::move(family)} {}

 private:
  ///
  auto ParseFromJson(const core::INode::ConstructorArgs& parsed_args,
                     const crude_json::value& json) const
      -> std::unique_ptr<core::INode> override {
    return std::make_unique<Node>(
        parsed_args, family_->GetPercentageIndex(),
        json.contains("reverse_order")
            ? json["reverse_order"].get<crude_json::boolean>()
            : false);
  }

  ///
  cpp::SafePtr<const Family> family_;
};

///
auto CreateFamilyNodeParser(cpp::SafePtr<const Family> family)
    -> std::unique_ptr<json::INodeParser> {
  return std::make_unique<NodeParser>(std::move(family));
}

///
constexpr auto kTypeName = "Coupler";

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
        parsed_id, json["percentage_index"].get<crude_json::number>());
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
    json["percentage_index"] =
        static_cast<crude_json::number>(family_->GetPercentageIndex());
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
    return GetLabelWithPercentage(family_->GetPercentageIndex());
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
auto CouplerFamilyGroup::CreateFamilies(core::IdGenerator& id_generator) const
    -> std::vector<std::unique_ptr<core::IFamily>> {
  auto families = std::vector<std::unique_ptr<core::IFamily>>{};

  for (auto percentage_index = 0;
       percentage_index < static_cast<int>(kDrops.size()); ++percentage_index) {
    families.emplace_back(std::make_unique<Family>(
        id_generator.Generate<core::FamilyId>(), percentage_index));
  }

  return families;
}

///
auto CouplerFamilyGroup::CreateFamilyParser() const
    -> std::unique_ptr<json::IFamilyParser> {
  return std::make_unique<FamilyParser>();
}
}  // namespace vh::ponc