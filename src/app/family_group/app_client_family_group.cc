/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "app_client_family_group.h"

#include <crude_json.h>
#include <imgui_node_editor.h>

#include <memory>
#include <optional>
#include <vector>

#include "core_i_node.h"
#include "core_id_generator.h"
#include "coreui_flow_pin_traits.h"
#include "coreui_i_family_traits.h"
#include "coreui_i_header_traits.h"
#include "coreui_i_node_traits.h"
#include "coreui_i_pin_traits.h"
#include "cpp_assert.h"
#include "cpp_safe_ptr.h"
#include "json_i_family_writer.h"
#include "json_i_node_parser.h"
#include "json_i_node_writer.h"
#include "style_tailwind.h"

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
constexpr auto kLabel = "Client";

///
class PinTraits : public coreui::FlowPinTraits {
 public:
  ///
  using FlowPinTraits::FlowPinTraits;

  ///
  auto GetLabel() const -> std::optional<coreui::PinLabel> override {
    return coreui::PinLabel{
        .text = kLabel,
        .color = style::Tailwind::GetColor(style::Tailwind::Color::kEmerald,
                                           style::Tailwind::Shade::k700)};
  }
};

///
class NodeUiTraits : public coreui::INodeTraits {
 public:
  ///
  explicit NodeUiTraits(cpp::SafePtr<const Node> node)
      : node_{std::move(node)} {}

  ///
  auto GetLabel() const -> std::string override { return kLabel; }

  ///
  auto CreatePinTraits() const
      -> std::vector<std::unique_ptr<coreui::IPinTraits>> override {
    auto pin_traits = std::vector<std::unique_ptr<coreui::IPinTraits>>{};
    pin_traits.reserve(1);

    pin_traits.emplace_back(std::make_unique<PinTraits>(
        core::INode::GetFirstPinOfKind(*node_, ne::PinKind::Input)));

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
auto CreateFamilyWriter() -> std::unique_ptr<json::IFamilyWriter>;
///
auto CreateFamilyUiTraits() -> std::unique_ptr<coreui::IFamilyTraits>;

///
class Family : public core::IFamily {
 public:
  ///
  explicit Family(core::FamilyId id) : IFamily{id} {}

  ///
  auto GetType() const -> std::optional<core::FamilyType> override {
    return core::FamilyType::kClient;
  }

  ///
  auto CreateNode(core::IdGenerator& id_generator) const
      -> std::unique_ptr<core::INode> override {
    return std::make_unique<Node>(core::INode::ConstructorArgs{
        .id = id_generator.Generate<ne::NodeId>(),
        .family_id = GetId(),
        .input_pin_id = id_generator.Generate<ne::PinId>()});
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
constexpr auto kTypeName = "Client";

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
auto ClientFamilyGroup::CreateFamilies(core::IdGenerator& id_generator) const
    -> std::vector<std::unique_ptr<core::IFamily>> {
  auto families = std::vector<std::unique_ptr<core::IFamily>>{};
  families.reserve(1);

  families.emplace_back(
      std::make_unique<Family>(id_generator.Generate<core::FamilyId>()));

  return families;
}

///
auto ClientFamilyGroup::CreateFamilyParser() const
    -> std::unique_ptr<json::IFamilyParser> {
  return std::make_unique<FamilyParser>();
}
}  // namespace vh::ponc