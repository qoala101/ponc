/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_DRAW_I_POPUP_H_
#define VH_DRAW_I_POPUP_H_

#include <string>

#include "app_state.h"
#include "cpp_interface.h"

namespace esc::draw {
class IPopup : public cpp::Interface {
 public:
  void Show();
  void Draw(frame::Frame &frame);

 private:
  virtual auto GetLabel() const -> std::string = 0;
  virtual void DrawItems(frame::Frame &frame) = 0;
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_POPUP_H_
