#ifndef VH_PONC_CPP_INTERFACE_H_
#define VH_PONC_CPP_INTERFACE_H_

namespace vh::ponc::cpp {
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
}  // namespace vh::ponc::cpp

#endif  // VH_PONC_CPP_INTERFACE_H_
