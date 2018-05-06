#include <exception>

using std::exception_ptr;

struct exception_arg_t {};
inline constexpr exception_arg_t exception_arg{};

template <typename Continuation, typename T>
concept bool FutureContinuation =
  requires (Continuation c, T value) { c(value); }
  ||
  requires (Continuation c)          { c(exception_arg, exception_ptr{}); };

