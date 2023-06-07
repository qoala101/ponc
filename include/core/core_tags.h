/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_CORE_TAGS_H_
#define VH_PONC_CORE_TAGS_H_

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
class Tags {
 public:
  ///
  explicit Tags(std::vector<std::shared_ptr<Tag>> tags = {});

  ///
  auto GetTags() const -> const std::vector<std::shared_ptr<Tag>> &;
  ///
  auto EmplaceTag(std::shared_ptr<Tag> tag) -> std::shared_ptr<Tag> &;

 private:
  ///
  std::vector<std::shared_ptr<Tag>> tags_{};
};
}  // namespace vh::ponc::core

#endif  // VH_PONC_CORE_TAGS_H_
