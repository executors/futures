// Proposed `Receiver`-related changes to P0443 based on P1053/P1055
// Reply-To: Bryce Adelstein Lelbach <brycelelbach@gmail.com>

// Ask #0: A receiver's error method shouldn't be an overload of the value
// method.

///////////////////////////////////////////////////////////////////////////////
// Before:
struct Receiver {
  // At least one of these is required (satisfied by `Callable`s):
  U operator()(T value);
  U operator()(exception_arg_t, std::exception_ptr error);
};

///////////////////////////////////////////////////////////////////////////////
// After:
struct Receiver {
  // At least one of these is required (satisfied by `Callable`s):
  U operator()(T value);
  U error(std::exception_ptr ptr);
};

