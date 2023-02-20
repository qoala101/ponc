#ifndef VH_JSON_I_NODE_PARSER_H_
#define VH_JSON_I_NODE_PARSER_H_

#include <imgui_node_editor.h>

#include <memory>

#include "core_i_node.h"
#include "cpp_interface.h"
#include "crude_json.h"

namespace esc::json {
// ---
class INodeParser : public cpp::Interface {
 public:
  // ---
  auto ParseFromJson [[nodiscard]] (const crude_json::value &json) const
      -> std::shared_ptr<core::INode>;

 private:
  // ---
  virtual auto ParseFromJson
      [[nodiscard]] (core::INode::ConstructorArgs parsed_args,
                     const crude_json::value &json) const
      -> std::shared_ptr<core::INode> = 0;
};
}  // namespace esc::json

#endif  // VH_JSON_I_NODE_PARSER_H_
