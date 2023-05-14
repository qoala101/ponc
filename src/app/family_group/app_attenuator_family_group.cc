#include "app_attenuator_family_group.h"

#include <cstdint>
#include <memory>

#include "core_i_node.h"
#include "core_id_generator.h"
#include "coreui_empty_pin_traits.h"
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
#include "json_i_node_writer.h"
#include "style_tailwind_colors.h"

namespace vh::ponc {
namespace {
class Node;

auto CreateNodeWriter(cpp::SafePtr<const Node> node)
    -> std::unique_ptr<json::INodeWriter>;

auto CreateNodeUiTraits(cpp::SafePtr<const Node> node)
    -> std::unique_ptr<coreui::INodeTraits>;

class Node : public core::INode {
 public:
  explicit Node(ConstructorArgs args, float drop = -15)
      : INode{std::move(args)}, drop_{drop} {}

  auto CreateWriter() const -> std::unique_ptr<json::INodeWriter> override {
    return CreateNodeWriter(safe_owner_.MakeSafe(this));
  }

  auto CreateUiTraits() const -> std::unique_ptr<coreui::INodeTraits> override {
    return CreateNodeUiTraits(safe_owner_.MakeSafe(this));
  }

  void SetInitialFlowValues(flow::NodeFlow& node_flow) const override {
    for (const auto output_pin_id : GetOutputPinIds()) {
      node_flow.output_pin_flows.at(output_pin_id.Get()) = drop_;
    }
  }

  auto GetDrop() const -> auto& { return drop_; }

 private:
  mutable float drop_{};
  cpp::SafeOwner safe_owner_{};
};

class NodeParser : public json::INodeParser {
 private:
  auto ParseFromJson(core::INode::ConstructorArgs parsed_args,
                     const crude_json::value& json) const
      -> std::unique_ptr<core::INode> override {
    return std::make_unique<Node>(std::move(parsed_args),
                                  json["drop"].get<crude_json::number>());
  }
};

class NodeWriter : public json::INodeWriter {
 public:
  explicit NodeWriter(cpp::SafePtr<const Node> node) : node_{std::move(node)} {}

 private:
  auto WriteToJson() const -> crude_json::value override {
    auto json = crude_json::value{};
    json["drop"] = node_->GetDrop();
    return json;
  }

  cpp::SafePtr<const Node> node_;
};

auto CreateNodeWriter(cpp::SafePtr<const Node> node)
    -> std::unique_ptr<json::INodeWriter> {
  return std::make_unique<NodeWriter>(std::move(node));
}

class DropPinTraits : public coreui::IPinTraits {
 public:
  explicit DropPinTraits(cpp::SafePtr<const Node> node)
      : node_{std::move(node)} {}

  auto GetPin() const -> std::variant<ne::PinId, ne::PinKind> override {
    return ne::PinKind::Input;
  }

  auto GetValue() const -> coreui::PinValueVariant override {
    return &node_->GetDrop();
  }

 private:
  cpp::SafePtr<const Node> node_;
};

class HeaderUiTraits : public coreui::IHeaderTraits {
 public:
  auto GetColor() const -> ImColor override {
    return style::TailwindColors::kIndigo700;
  }
};

class NodeUiTraits : public coreui::INodeTraits {
 public:
  explicit NodeUiTraits(cpp::SafePtr<const Node> node)
      : node_{std::move(node)} {}

  auto GetLabel() const -> std::string override { return "Attenuator"; }

  auto CreateHeaderTraits() const
      -> std::optional<std::unique_ptr<coreui::IHeaderTraits>> override {
    return std::make_unique<HeaderUiTraits>();
  }

  auto CreatePinTraits() const
      -> std::vector<std::unique_ptr<coreui::IPinTraits>> override {
    auto pin_traits = std::vector<std::unique_ptr<coreui::IPinTraits>>{};
    pin_traits.emplace_back(
        std::make_unique<coreui::FlowPinTraits>(*node_->GetInputPinId()));
    pin_traits.emplace_back(std::make_unique<DropPinTraits>(node_));
    pin_traits.emplace_back(
        std::make_unique<coreui::EmptyPinTraits>(ne::PinKind::Output));
    pin_traits.emplace_back(std::make_unique<coreui::FlowPinTraits>(
        node_->GetOutputPinIds().front()));
    return pin_traits;
  }

 private:
  cpp::SafePtr<const Node> node_;
};

auto CreateNodeUiTraits(cpp::SafePtr<const Node> node)
    -> std::unique_ptr<coreui::INodeTraits> {
  return std::make_unique<NodeUiTraits>(std::move(node));
}

class Family;

auto CreateFamilyWriter(cpp::SafePtr<const Family> family)
    -> std::unique_ptr<json::IFamilyWriter>;

auto CreateFamilyUiTraits(cpp::SafePtr<const Family> family)
    -> std::unique_ptr<coreui::IFamilyTraits>;

class Family : public core::IFamily {
 public:
  explicit Family(core::FamilyId id) : IFamily{id} {}

  auto CreateNode(core::IdGenerator& id_generator) const
      -> std::unique_ptr<core::INode> override {
    return std::make_unique<Node>(core::INode::ConstructorArgs{
        .id = id_generator.Generate<ne::NodeId>(),
        .family_id = GetId(),
        .input_pin_id = id_generator.Generate<ne::PinId>(),
        .output_pin_ids = id_generator.GenerateN<ne::PinId>(1)});
  }

  auto CreateNodeParser() const -> std::unique_ptr<json::INodeParser> override {
    return std::make_unique<NodeParser>();
  }

  auto CreateWriter() const -> std::unique_ptr<json::IFamilyWriter> override {
    return CreateFamilyWriter(safe_owner_.MakeSafe(this));
  }

  auto CreateUiTraits() const
      -> std::unique_ptr<coreui::IFamilyTraits> override {
    return CreateFamilyUiTraits(safe_owner_.MakeSafe(this));
  }

 private:
  cpp::SafeOwner safe_owner_{};
};

constexpr auto kTypeName = "Attenuator";

class FamilyParser : public json::IFamilyParser {
 public:
  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto ParseFromJson(core::FamilyId parsed_id,
                     const crude_json::value& /*unused*/) const
      -> std::unique_ptr<core::IFamily> override {
    return std::make_unique<Family>(parsed_id);
  }
};

class FamilyWriter : public json::IFamilyWriter {
 public:
  explicit FamilyWriter(cpp::SafePtr<const Family> family)
      : family_{std::move(family)} {}

 private:
  auto GetTypeName() const -> std::string override { return kTypeName; }

  cpp::SafePtr<const Family> family_;
};

auto CreateFamilyWriter(cpp::SafePtr<const Family> family)
    -> std::unique_ptr<json::IFamilyWriter> {
  return std::make_unique<FamilyWriter>(std::move(family));
}

class FamilyUiTraits : public coreui::IFamilyTraits {
 public:
  explicit FamilyUiTraits(cpp::SafePtr<const Family> family)
      : family_{std::move(family)} {}

  auto GetLabel() const -> std::string override { return "Attenuator"; }

 private:
  cpp::SafePtr<const Family> family_;
};

auto CreateFamilyUiTraits(cpp::SafePtr<const Family> family)
    -> std::unique_ptr<coreui::IFamilyTraits> {
  return std::make_unique<FamilyUiTraits>(std::move(family));
}
}  // namespace

auto AttenuatorFamilyGroup::CreateFamilies(core::IdGenerator& id_generator)
    const -> std::vector<std::unique_ptr<core::IFamily>> {
  auto families = std::vector<std::unique_ptr<core::IFamily>>{};
  families.emplace_back(
      std::make_unique<Family>(id_generator.Generate<core::FamilyId>()));
  return families;
}

auto AttenuatorFamilyGroup::CreateFamilyParser() const
    -> std::unique_ptr<json::IFamilyParser> {
  return std::make_unique<FamilyParser>();
}
}  // namespace vh::ponc