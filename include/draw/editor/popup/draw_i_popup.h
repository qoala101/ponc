#ifndef VH_DRAW_I_POPUP_H_
#define VH_DRAW_I_POPUP_H_

#include <string>

#include "cpp_interface.h"
#include "frame_node.h"

namespace esc::draw {
class IPopup : public cpp::Interface {
 public:
  void Show();
  void Draw(coreui::Frame &frame);

 private:
  virtual auto GetLabel() const -> std::string = 0;
  virtual void DrawItems(coreui::Frame &frame) = 0;
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_POPUP_H_
