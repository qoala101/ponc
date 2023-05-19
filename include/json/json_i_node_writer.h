/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_JSON_I_NODE_WRITER_H_
#define VH_PONC_JSON_I_NODE_WRITER_H_

#include <crude_json.h>

#include "core_i_node.h"
#include "cpp_non_copyable.h"

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
