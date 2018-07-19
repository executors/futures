// Proposed `Receiver`-related changes to P0443 based on P1053/P1055
// Reply-To: Bryce Adelstein Lelbach <brycelelbach@gmail.com>

// Ask #6: Receiver's value method shouldn't be the call operator.

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

exec.execute([] { /* ... */ });
auto f = exec.twoway_execute([] { /* ... */ });
auto f = exec.then_execute([] { /* ... */ }, pred);

///////////////////////////////////////////////////////////////////////////////
// After:
// `execute` takes:
struct VoidNoneReceiver /* AKA a `Callable` */ {
  void done();
};
// `twoway_execute` takes:
struct NoneReceiver /* AKA a `Callable` */ {
  U done();
};
// `then_execute` takes:
struct Receiver {
  // At least one of these is required (satisfied by `Callable`s):
  U value(T value);
  U operator()(exception_arg_t, std::exception_ptr error);
};

// Using P1054-style `on_value`:
exec.execute(on_value([] { /* ... */ }));
auto f = exec.twoway_execute(on_value([] { /* ... */ }));
auto g = exec.then_execute(on_value([] { /* ... */ }), pred);

///////////////////////////////////////////////////////////////////////////////

// NOTE: This change makes the most sense if done in conjunction with ask #1 and
// ask #4.

///////////////////////////////////////////////////////////////////////////////
// Before (with ask #1 and ask #4):
// `execute` takes:
struct VoidNoneReceiver {
  // BOTH of these are required (NOT satisfied by `Callable`s):
  void operator()();
  void error(exception_arg_t, std::exception_ptr error);
};
// `twoway_execute` takes:
struct NoneReceiver {
  // BOTH of these are required (NOT satisfied by `Callable`s):
  U operator()();
  U error(exception_arg_t, std::exception_ptr error);
};
struct Receiver {
  // BOTH of these are required (NOT satisfied by `Callable`s):
  U operator()(T value);
  U error(exception_arg_t, std::exception_ptr error);
};

// Using P1054-style `on_value`:
exec.execute(on_value([] { /* ... */ });
auto f = exec.twoway_execute(on_value([] { /* ... */ }));
auto g = exec.then_execute(on_value([] { /* ... */ }, pred));

///////////////////////////////////////////////////////////////////////////////
// After (with ask #1 and ask #4):
// `execute` takes:
struct VoidNoneReceiver /* AKA a `Callable` */ {
  void done();
  void error(exception_arg_t, std::exception_ptr error);
};
// `twoway_execute` takes:
struct NoneReceiver /* AKA a `Callable` */ {
  U done();
  U error(exception_arg_t, std::exception_ptr error);
};
// `then_execute` takes:
struct Receiver {
  // At least one of these is required (satisfied by `Callable`s):
  U value(T value);
  U error(exception_arg_t, std::exception_ptr error);
};

// Using P1054-style `on_value`:
exec.execute(on_value([] { /* ... */ }));
auto f = exec.twoway_execute(on_value([] { /* ... */ }));
auto g = exec.then_execute(on_value([] { /* ... */ }), pred);

