// Proposed `Receiver`-related changes to P0443 based on P1053/P1055
// Reply-To: Bryce Adelstein Lelbach <brycelelbach@gmail.com>

// Ask 5: Non-dependent execution functions should take `ExecutorReceiver`s
// (e.g. `Callable`s that take an `Executor` parameter) instead of
// `VoidNoneReceiver`s (nullary `Callable`s). An executor is free to pass any
// `Executor` to a `ExecutorReceiver`, not necessarily themselves.

///////////////////////////////////////////////////////////////////////////////
// Before:
struct VoidNoneReceiver /* AKA a `Callable` */ {
  void operator()();
};
struct OneWayExecutor {
  void execute(VoidNoneReceiver r);
};

exec.execute([] { /* ... */ });

///////////////////////////////////////////////////////////////////////////////
// After:
struct ExecutorReceiver {
  void operator()(Executor exec);
};
struct OneWayExecutor {
  void execute(ExecutorReceiver r);
};

exec.execute([] (Executor e) { /* ... */ });

