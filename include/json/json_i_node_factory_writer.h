#ifndef VH_JSON_I_NODE_FACTORY_WRITER_H_
#define VH_JSON_I_NODE_FACTORY_WRITER_H_

#include "core_i_node_factory.h"
#include "cpp_interface.h"
#include "crude_json.h"

namespace esc::json {
// ---
class INodeFactoryWriter : public cpp::Interface {
 public:
  // ---
  auto WriteToJson [[nodiscard]] (const core::INodeFactory &factory) const
      -> crude_json::value;

 private:
  // ---
  virtual auto GetTypeName [[nodiscard]] () const -> std::string = 0;
  // ---
  virtual auto WriteToJson [[nodiscard]] () const -> crude_json::value = 0;
};
}  // namespace esc::json

#endif  // VH_JSON_I_NODE_FACTORY_WRITER_H_
