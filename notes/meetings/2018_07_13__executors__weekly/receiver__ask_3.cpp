// Proposed `Receiver`-related changes to P0443 based on P1053/P1055
// Reply-To: Bryce Adelstein Lelbach <brycelelbach@gmail.com>

// Ask 3: All execution functions should take receivers. A non- or
// maybe-blocking executor may choose to report system failures (shutdown,
// launch failure, etc) that occur after the execution function returns via the
// receiver's error method.

///////////////////////////////////////////////////////////////////////////////
// Before:
// `execute` takes:
struct VoidNoneReceiver /* AKA a `Callable` */ {
  void operator()();
};
// `twoway_execute` takes:
struct NoneReceiver /* AKA a `Callable` */ {
  U operator()();
};
// `then_execute` takes:
struct Receiver {
  // At least one of these is required (satisfied by `Callable`s):
  U operator()(T value);
  U operator()(exception_arg_t, std::exception_ptr error);
};

///////////////////////////////////////////////////////////////////////////////
// After:
// `execute` takes:
struct VoidNoneReceiver {
  // At least one of these is required (satisfied by `Callable`s):
  void operator()();
  void operator()(exception_arg_t, std::exception_ptr error);
};
// `twoway_execute` takes:
struct NoneReceiver /* AKA a `Callable` */ {
  // At least one of these is required (satisfied by `Callable`s):
  U operator()();
  U operator()(exception_arg_t, std::exception_ptr error);
};
// `then_execute` takes:
struct Receiver {
  // At least one of these is required (satisfied by `Callable`s):
  U operator()(T value);
  U operator()(exception_arg_t, std::exception_ptr error);
};

