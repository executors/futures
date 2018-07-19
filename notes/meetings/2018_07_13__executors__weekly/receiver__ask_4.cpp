// Proposed `Receiver`-related changes to P0443 based on P1053/P1055
// Reply-To: Bryce Adelstein Lelbach <brycelelbach@gmail.com>

// Ask #4: Receivers should be required to have both the value method and the
// error method. P1054-style helper functions should be added to ease
// construction of receivers.

///////////////////////////////////////////////////////////////////////////////
// Before:
struct Receiver {
  // At least one of these is required (satisfied by `Callable`s):
  U operator()(T value);
  U operator()(exception_arg_t, std::exception_ptr error);
};

auto f = exec.then_execute([] { /* ... */ }, pred);

///////////////////////////////////////////////////////////////////////////////
// After:
struct Receiver {
  // BOTH of these are required (NOT satisfied by `Callable`s):
  U operator()(T value);
  U operator()(exception_arg_t, std::exception_ptr error);
};

// Using P1054-style `on_value`:
auto f = exec.then_execute(on_value([] { /* ... */ }), pred);

///////////////////////////////////////////////////////////////////////////////

// NOTE: This would apply to all execution functions and all types of receivers
// if ask #3 is accepted.

///////////////////////////////////////////////////////////////////////////////
// Before (with ask #3):
// `execute` takes:
struct VoidNoneReceiver {
  // At least one of these is required (satisfied by `Callable`s):
  void operator()();
  void operator()(exception_arg_t, std::exception_ptr error);
};
// `twoway_execute` takes:
struct NoneReceiver {
  // At least one of these is required (satisfied by `Callable`s):
  U operator()();
  U operator()(exception_arg_t, std::exception_ptr error);
};

exec.execute([] { /* ... */ });
auto f = exec.twoway_execute([] { /* ... */ });

///////////////////////////////////////////////////////////////////////////////
// After (with ask #3):
// `execute` takes:
struct VoidNoneReceiver {
  // BOTH of these are required (NOT satisfied by `Callable`s):
  void operator()();
  void operator()(exception_arg_t, std::exception_ptr error);
};
// `twoway_execute` takes:
struct NoneReceiver {
  // BOTH of these are required (NOT satisfied by `Callable`s):
  U operator()();
  U operator()(exception_arg_t, std::exception_ptr error);
};

// Using P1054-style `on_value`:
exec.execute(on_value([] { /* ... */ }));
auto f = exec.twoway_execute(on_value([] { /* ... */ }));

