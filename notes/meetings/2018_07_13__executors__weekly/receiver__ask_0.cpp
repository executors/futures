// Proposed `Receiver`-related changes to P0443 based on P1053/P1055
// Reply-To: Bryce Adelstein Lelbach <brycelelbach@gmail.com>

// Ask #0: A receiver's error method should take a generic error type instead
// of an `std::exception_ptr`.

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
  U operator()(exception_arg_t, E error);
};

