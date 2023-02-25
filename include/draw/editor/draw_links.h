#ifndef VH_DRAW_LINKS_H_
#define VH_DRAW_LINKS_H_

#include "frame_node.h"

namespace esc::draw {
void DrawLinks(const std::vector<coreui::Link> &links,
               const std::optional<coreui::Curve> &curve);
}  // namespace esc::draw

#endif  // VH_DRAW_LINKS_H_
