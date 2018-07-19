// Proposed `Receiver`-related changes to P0443 based on P1053/P1055
// Reply-To: Bryce Adelstein Lelbach <brycelelbach@gmail.com>

// Lexicon:
//   Receiver == Promise == (Future)Continuation == Callable == Task == Work Item
//   E.g. the things that executors invoke.

// P0443 and P1054 Type Requirements for Work Items (Status Quo)

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

// P1054 helper functions for constructing receivers:
Receiver<T> on_value(Callable<T> value_f);
Receiver<T> on_error(Callable<T> error_f);
Receiver<T> on_value_or_error(Callable<T> value_f, Callable<T> error_f);

