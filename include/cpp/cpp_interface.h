#ifndef VH_CPP_INTERFACE_H_
#define VH_CPP_INTERFACE_H_

namespace esc::cpp {
///
class Interface {
 public:
  ///
  Interface(const Interface &) = delete;
  ///
  Interface(Interface &&) noexcept = delete;

  ///
  auto operator=(const Interface &) -> Interface & = delete;
  ///
  auto operator=(Interface &&) noexcept -> Interface & = delete;

  ///
  virtual ~Interface() = default;

 protected:
  ///
  Interface() = default;
};
}  // namespace esc::cpp

#endif  // VH_CPP_INTERFACE_H_
