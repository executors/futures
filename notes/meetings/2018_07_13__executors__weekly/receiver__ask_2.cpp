// Proposed `Receiver`-related changes to P0443 based on P1053/P1055
// Reply-To: Bryce Adelstein Lelbach <brycelelbach@gmail.com>

// Ask 2: The value and error methods of a receiver should not be required
// to have the same return type.

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
  V operator()(exception_arg_t, std::exception_ptr error);
};

///////////////////////////////////////////////////////////////////////////////

// NOTE: If we move towards a model where we have a single one-way execution
// function, this problem will solve itself, as the only types of receivers we
// have will be ones that return `void`.

