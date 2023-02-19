/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_DRAW_I_POPUP_H_
#define VH_DRAW_I_POPUP_H_

#include <string>

#include "app_state.h"
#include "cpp_interface.h"

namespace esc::draw {
// ---
class IPopup : public cpp::Interface {
 public:
  // ---
  void Show();
  // ---
  void Draw(const AppState &app_state);

 private:
  // ---
  virtual auto GetLabel [[nodiscard]] () const -> std::string = 0;
  // ---
  virtual void DrawItems(const AppState &app_state) = 0;
};
}  // namespace esc::draw

#endif  // VH_DRAW_I_POPUP_H_
