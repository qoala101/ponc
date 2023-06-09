/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_CORE_TAG_H_
#define VH_PONC_CORE_TAG_H_

#include <imgui.h>

#include <memory>
#include <vector>

namespace vh::ponc::core {
///
struct Tag {
  ///
  std::string name{};
  ///
  ImColor color{};
};

///
using Tags = std::vector<std::shared_ptr<Tag>>;
}  // namespace vh::ponc::core

#endif  // VH_PONC_CORE_TAG_H_
