#include "core_hub_family_group.h"

#include <memory>
#include <optional>
#include <vector>

#include "core_i_family.h"
#include "core_i_node.h"
#include "core_id_generator.h"
#include "coreui_flow_pin_traits.h"
#include "coreui_i_family_traits.h"
#include "coreui_i_header_traits.h"
#include "coreui_i_node_traits.h"
#include "coreui_i_pin_traits.h"
#include "cpp_assert.h"
#include "cpp_safe_ptr.h"
#include "crude_json.h"
#include "imgui_node_editor.h"
#include "json_i_family_writer.h"
#include "json_i_node_parser.h"
#include "json_i_node_writer.h"

namespace vh::ponc::core {
namespace {
class Node;

auto CreateNodeWriter(cpp::SafePtr<const Node> node)
    -> std::unique_ptr<json::INodeWriter>;

auto CreateNodeUiTraits(cpp::SafePtr<const Node> node)
    -> std::unique_ptr<coreui::INodeTraits>;

class Node : public core::INode {
 public:
  explicit Node(ConstructorArgs args) : INode{std::move(args)} {}

  auto CreateWriter() const -> std::unique_ptr<json::INodeWriter> override {
    return CreateNodeWriter(safe_owner_.MakeSafe(this));
  }

  auto CreateUiTraits() const -> std::unique_ptr<coreui::INodeTraits> override {
    return CreateNodeUiTraits(safe_owner_.MakeSafe(this));
  }

 private:
  cpp::SafeOwner safe_owner_{};
};

class NodeWriter : public json::INodeWriter {
 public:
  explicit NodeWriter(cpp::SafePtr<const Node> node) : node_{std::move(node)} {}

 private:
  cpp::SafePtr<const Node> node_;
};

auto CreateNodeWriter(cpp::SafePtr<const Node> node)
    -> std::unique_ptr<json::INodeWriter> {
  return std::make_unique<NodeWriter>(std::move(node));
}

class HeaderUiTraits : public coreui::IHeaderTraits {
 public:
  explicit HeaderUiTraits(cpp::SafePtr<const Node> node)
      : node_{std::move(node)} {}

  auto GetColor() const -> ImColor override { return {1.F, 1.F, 1.F}; }

 private:
  cpp::SafePtr<const Node> node_;
};

class NodeUiTraits : public coreui::INodeTraits {
 public:
  explicit NodeUiTraits(cpp::SafePtr<const Node> node)
      : node_{std::move(node)} {}

  auto GetLabel() const -> std::string override {
    return "Hub 1x" + std::to_string(node_->GetOutputPinIds().size());
  }

  auto CreateHeaderTraits() const
      -> std::optional<std::unique_ptr<coreui::IHeaderTraits>> override {
    return std::make_unique<HeaderUiTraits>(node_);
  }

  auto CreatePinTraits() const
      -> std::vector<std::unique_ptr<coreui::IPinTraits>> override {
    auto pin_traits = std::vector<std::unique_ptr<coreui::IPinTraits>>{};

    pin_traits.emplace_back(
        std::make_unique<coreui::FlowPinTraits>(*node_->GetInputPinId()));

    for (const auto pin_id : node_->GetOutputPinIds()) {
      pin_traits.emplace_back(std::make_unique<coreui::FlowPinTraits>(pin_id));
    }

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

auto CreateFamilyNodeParser(cpp::SafePtr<const Family> family)
    -> std::unique_ptr<json::INodeParser>;

auto CreateFamilyWriter(cpp::SafePtr<const Family> family)
    -> std::unique_ptr<json::IFamilyWriter>;

auto CreateFamilyUiTraits(cpp::SafePtr<const Family> family)
    -> std::unique_ptr<coreui::IFamilyTraits>;

class Family : public core::IFamily {
 public:
  explicit Family(core::FamilyId id, int num_output_pins)
      : IFamily{id}, num_output_pins_{num_output_pins} {}

  auto GetType() const -> std::optional<FamilyType> override {
    return FamilyType::kHub;
  }

  auto CreateNode(core::IdGenerator& id_generator) const
      -> std::unique_ptr<core::INode> override {
    return std::make_unique<Node>(core::INode::ConstructorArgs{
        .id = id_generator.Generate<ne::NodeId>(),
        .family_id = GetId(),
        .input_pin_id = id_generator.Generate<ne::PinId>(),
        .output_pin_ids = id_generator.GenerateN<ne::PinId>(num_output_pins_)});
  }

  auto CreateNodeParser() const -> std::unique_ptr<json::INodeParser> override {
    return CreateFamilyNodeParser(safe_owner_.MakeSafe(this));
  }

  auto CreateWriter() const -> std::unique_ptr<json::IFamilyWriter> override {
    return CreateFamilyWriter(safe_owner_.MakeSafe(this));
  }

  auto CreateUiTraits() const
      -> std::unique_ptr<coreui::IFamilyTraits> override {
    return CreateFamilyUiTraits(safe_owner_.MakeSafe(this));
  }

  auto GetNumOutputPins() const { return num_output_pins_; }

 private:
  int num_output_pins_{};
  cpp::SafeOwner safe_owner_{};
};

class NodeParser : public json::INodeParser {
 public:
  explicit NodeParser(cpp::SafePtr<const Family> family)
      : family_{std::move(family)} {}

 private:
  auto ParseFromJson(core::INode::ConstructorArgs parsed_args,
                     const crude_json::value& json) const
      -> std::unique_ptr<core::INode> override {
    return std::make_unique<Node>(std::move(parsed_args));
  }

  cpp::SafePtr<const Family> family_;
};

auto CreateFamilyNodeParser(cpp::SafePtr<const Family> family)
    -> std::unique_ptr<json::INodeParser> {
  return std::make_unique<NodeParser>(std::move(family));
}

constexpr auto kTypeName = "Hub";

class FamilyParser : public json::IFamilyParser {
 public:
  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto ParseFromJson(core::FamilyId parsed_id,
                     const crude_json::value& json) const
      -> std::unique_ptr<core::IFamily> override {
    return std::make_unique<Family>(
        parsed_id, json["num_output_pins"].get<crude_json::number>());
  }
};

class FamilyWriter : public json::IFamilyWriter {
 public:
  explicit FamilyWriter(cpp::SafePtr<const Family> family)
      : family_{std::move(family)} {}

 private:
  auto GetTypeName() const -> std::string override { return kTypeName; }

  auto WriteToJson() const -> crude_json::value override {
    auto json = crude_json::value{};
    json["num_output_pins"] =
        static_cast<crude_json::number>(family_->GetNumOutputPins());
    return json;
  }

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

  auto GetLabel() const -> std::string override {
    return "Hub 1x" + std::to_string(family_->GetNumOutputPins());
  }

  auto GetGroupLabel() const -> std::string override { return "Hub"; }

 private:
  cpp::SafePtr<const Family> family_;
};

auto CreateFamilyUiTraits(cpp::SafePtr<const Family> family)
    -> std::unique_ptr<coreui::IFamilyTraits> {
  return std::make_unique<FamilyUiTraits>(std::move(family));
}
}  // namespace

auto HubFamilyGroup::CreateFamilies(core::IdGenerator& id_generator) const
    -> std::vector<std::unique_ptr<core::IFamily>> {
  auto families = std::vector<std::unique_ptr<core::IFamily>>{};

  for (auto num_output_pins = 1; num_output_pins <= 32; ++num_output_pins) {
    families.emplace_back(std::make_unique<Family>(
        id_generator.Generate<core::FamilyId>(), num_output_pins));
  }

  return families;
}

auto HubFamilyGroup::CreateFamilyParser() const
    -> std::unique_ptr<json::IFamilyParser> {
  return std::make_unique<FamilyParser>();
}
}  // namespace vh::ponc::core