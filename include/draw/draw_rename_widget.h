/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_RENAME_WIDGET_H_
#define VH_PONC_DRAW_RENAME_WIDGET_H_

#include <string_view>

#include "draw_string_buffer.h"

namespace vh::ponc::draw {
///
auto DrawRenameWidget(std::string_view label, StringBuffer &name_buffer)
    -> bool;
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_RENAME_WIDGET_H_
