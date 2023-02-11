#ifndef VH_JSON_I_NODE_FACTORY_PARSER_H_
#define VH_JSON_I_NODE_FACTORY_PARSER_H_

#include <memory>

#include "core_i_node.h"
#include "core_i_node_factory.h"
#include "cpp_interface.h"
#include "crude_json.h"

namespace esc::json {
// ---
class INodeFactoryParser : public cpp::Interface {
 public:
  // ---
  auto TryToParseFromJson [[nodiscard]] (const crude_json::value &json) const
      -> std::optional<std::shared_ptr<core::INodeFactory>>;

 private:
  // ---
  virtual auto GetTypeName [[nodiscard]] () const -> std::string = 0;
  // ---
  virtual auto ParseFromJson
      [[nodiscard]] (std::vector<std::shared_ptr<core::INode>> parsed_nodes,
                     const crude_json::value &json) const
      -> std::shared_ptr<core::INodeFactory> = 0;
};
}  // namespace esc::json

#endif  // VH_JSON_I_NODE_FACTORY_PARSER_H_
