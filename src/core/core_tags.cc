/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "core_tags.h"

namespace vh::ponc::core {
///
Tags::Tags(std::vector<std::shared_ptr<Tag>> tags) : tags_{std::move(tags)} {}

///
auto Tags::GetTags() const -> const std::vector<std::shared_ptr<Tag>>& {
  return tags_;
}

///
auto Tags::EmplaceTag(std::shared_ptr<Tag> tag) -> std::shared_ptr<Tag>& {
  return tags_.emplace_back(std::move(tag));
}
}  // namespace vh::ponc::core