#ifndef VH_JSON_NODE_SERIALIZER_H_
#define VH_JSON_NODE_SERIALIZER_H_

#include <imgui_node_editor.h>

#include <memory>

#include "crude_json.h"

namespace ne = ax::NodeEditor;

namespace esc {
namespace core {
class INode;
}  // namespace core

namespace json {
// ---
// NOLINTNEXTLINE(*-special-member-functions)
class INodeParser {
 public:
  virtual ~INodeParser() = default;

  // ---
  auto TryToParseFromJson [[nodiscard]] (const crude_json::value &json) const
      -> std::optional<std::shared_ptr<core::INode>>;

 protected:
  INodeParser() = default;

 private:
  // ---
  virtual auto GetTypeName [[nodiscard]] () const -> std::string = 0;
  // ---
  virtual auto ParseFromJson
      [[nodiscard]] (ne::NodeId parsed_node_id,
                     std::vector<ne::PinId> parsed_pin_ids,
                     const crude_json::value &json) const
      -> std::shared_ptr<core::INode> = 0;
};

// ---
// NOLINTNEXTLINE(*-special-member-functions)
class INodeWriter {
 public:
  virtual ~INodeWriter() = default;

  // ---
  auto WriteToJson [[nodiscard]] (const core::INode &node) const
      -> crude_json::value;

 protected:
  INodeWriter() = default;

 private:
  // ---
  virtual auto GetTypeName [[nodiscard]] () const -> std::string = 0;
  // ---
  virtual auto WriteToJson [[nodiscard]] () const -> crude_json::value = 0;
};
}  // namespace json
}  // namespace esc

#endif  // VH_JSON_NODE_SERIALIZER_H_
