#ifndef VH_PONC_JSON_I_NODE_WRITER_H_
#define VH_PONC_JSON_I_NODE_WRITER_H_

#include <memory>

#include "core_i_node.h"
#include "cpp_non_copyable.h"
#include "crude_json.h"

namespace vh::ponc::json {
///
class INodeWriter : public cpp::NonCopyable {
 public:
  ///
  auto WriteToJson(const core::INode &node) const -> crude_json::value;

 private:
  ///
  virtual auto WriteToJson() const -> crude_json::value;
};
}  // namespace vh::ponc::json

#endif  // VH_PONC_JSON_I_NODE_WRITER_H_
