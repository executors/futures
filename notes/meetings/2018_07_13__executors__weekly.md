ISO C++ Futures
===============

Weekly US Timezone Meeting - Notes - 2018-07-18
-----------------------------------------------

**Kindly forgive and identify any mistakes and omissions in these notes.**

Attending:
- Bryce Adelstein Lelbach (Note Taker)
- Christopher Kohlhoff
- David Hollman
- Jared Hoberock
- Lee Howes
- Mathias Stearn
- **???**

Agenda:
- (from Bryce) Review of proposed `Receiver`-related changes to P0443 and P1054:
- (from Chris K) Discuss experiments with `then_execute` alternatives.

### Discussion

#### (from Bryce) Review of proposed `Receiver`-related changes to P0443 and P1054:

- Bryce: My goal is to try and identify a concrete series of atomic changes that can be applied (in order) based on P1053 and P1055. This will let us evaluate each change and its implications individually.
- Bryce: I'm not necessarily advocating for all of these changes, just bringing them here for us to evaluate as a group.
- Bryce: The plan I came up with was:
  - 0) Add `None`/`SingleReceiver` concepts and replace raw callables/continuations with it
  - 1) Add P1054 style `Receiver` wrappers.
  - 2) Add `None`/`SingleSender` concepts.
  - 3) Replace `then_execute` with `via_execute` or an alternative.
  - 4) Refactor two-way execution in terms of one-way execution.
  - 5) Refactor bulk execution in terms of one-way execution.
  - 6) Replace executor concepts with sender concepts.
- Bryce: I thought we should start with the receiver concepts because we did similar work in P0443 and P1054 pre-Rapperswil to define `(bulk_)then_execute` error handling worked.

Lexicon:
- Receiver == Promise == (Future)Continuation == Callable == Task == Work Item
- E.g. the things that executors invoke.

**Status Quo (P0443 and P1054):**
```
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
```


**Ask 0:** A receiver's error method shouldn't be an overload of the value method.

[EXECUTORS ISSUE #408](https://github.com/executors/executors/issues/408)

```
// Before:
struct Receiver {
  // At least one of these is required (satisfied by `Callable`s):
  U operator()(T value);
  U operator()(exception_arg_t, std::exception_ptr error);
};


// After:
struct Receiver {
  // At least one of these is required (satisfied by `Callable`s):
  U operator()(T value);
  U error(std::exception_ptr ptr);
};
```

- Bryce, Lee: As P1053 points out, it's very easy today to accidentally forget the `exception_arg_t` parameter when writing your error method. You'll then be surprised when your code compiles, but your "error" method is never called.
- **There was consensus to accept this.**


**Ask 1:** A receiver's error method should take a generic error type instead of an `std::exception_ptr`.

[EXECUTORS ISSUE #409](https://github.com/executors/executors/issues/409)

```
// Before:
struct Receiver {
  // At least one of these is required (satisfied by `Callable`s):
  U operator()(T value);
  U operator()(exception_arg_t, std::exception_ptr error);
};


// After:
struct Receiver {
  // At least one of these is required (satisfied by `Callable`s):
  U operator()(T value);
  U operator()(exception_arg_t, E error);
};
```

- Bryce: Places additional burden on authors of receiver.
- Kirk: The error type should be required to be `SemiMovable`.
- Jared: Would you be able to just write an overload for one concrete type?
- Bryce: This is related to whether the error method should be required.
- Bryce: Two options if no error method matches:
  - Compile fail.
  - Propagate.
- Chris K: What's the impact on the polymorphic executor?


**Ask 2:** The value and error methods of a receiver should not be required to have the same return type.

[EXECUTORS ISSUE #410](https://github.com/executors/executors/issues/410)

```
// Before:
struct Receiver {
  // At least one of these is required (satisfied by `Callable`s):
  U operator()(T value);
  U operator()(exception_arg_t, std::exception_ptr error);
};


// After:
struct Receiver {
  // At least one of these is required (satisfied by `Callable`s):
  U operator()(T value);
  V operator()(exception_arg_t, std::exception_ptr error);
};
```

- Bryce: Lee is the champion for this one, this is inspired by P1053.
- Bryce: If we move towards a model where we have a single one-way execution function, this problem will solve itself, as the only types of receivers we have will be ones that return `void`.
- Jared, Bryce: What type of future do you get from an `on_error`. `then_execute(on_error(e), f)` -> `future<?>`.
- Bryce: Maybe `future<void>`?
- Lee: I think so.
- Jared: I'd like to see more motivation/use cases.
- **Lee: I will come back with examples.**


**Ask 3:** All execution functions should take receivers (AKA how do executors deal with asynchronous system errors and allow users to customize that handling?).

[EXECUTORS ISSUE #411](https://github.com/executors/executors/issues/411)

```
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
```

- Bryce: P0443 is consistent with the IS; exceptions that escape from a thread of execution lead to `std::terminate`.
- Lee: The example is a non-blocking launch + a shutdown request that arrives before the work is started.
- Chris K: This should be handled through a different channel.
- Chris K: The problem is that the error function is not called in the right execution agent. But for `then_execute`, it will be called in the execution agent. You are using the same channel for two different things.
- Bryce: For `then_execute`, can this be called due to a launch error.
- Bryce: The proposal is that this error method would be used to handle both "system" errors and errors contained in tasks?
- Bryce: This means that error is invoked on an unspecified execution agent. Remember the Concurrency TS v1.
- Jared: Is an executor allowed to throw for synchronous errors? Or does it have to use the error method?
- Bryce: If you don't always use the error method, changes in the implementation of an executor can change the error handling path. I think the error method would always have to be used.
- Lee: Not just about launch errors, also things like shutdown, migration, cancellation, etc.
- Chris K: We want two channels, one for dependency errors, one for system errors.
- **Bryce: Idea - have two channels (or a way to signal error category) on receivers.**
- Lee: What if the preceding task forwarded the error to it's dependency?
- Bryce: Two questions for Jared and Chris K:
  - Should we provide some way of providing an error handling hook for asynchronous system errors?
  - Would you feel differently about this if there was a way to distinguish the error category (system or predecessor)?
- Jared: Could this be done with properties?
- Lee, Kirk: Executors do not have to report system errors with the error method even if it exists.
- Lee, Kirk: This is needed for error propagation if we rebase `then_execute` on `execute`.
- Bryce: So a non- or maybe-blocking executor may choose to, but doesn't have to, report system failures (shutdown, launch failure, etc) that occur after the execution function returns via the receiver's error method?
- Bryce: That makes error handling inconsistency in generic code.


**Ask 4:** Receivers should be required to have both the value method and the error method.

[EXECUTORS ISSUE #412](https://github.com/executors/executors/issues/412)

```
// Before:
struct Receiver {
  // At least one of these is required (satisfied by `Callable`s):
  U operator()(T value);
  U operator()(exception_arg_t, std::exception_ptr error);
};

auto f = exec.then_execute([] { /* ... */ }, pred);


// After:
struct Receiver {
  // BOTH of these are required (NOT satisfied by `Callable`s):
  U operator()(T value);
  U operator()(exception_arg_t, std::exception_ptr error);
};

// Using P1054-style `on_value`:
auto f = exec.then_execute(on_value([] { /* ... */ }), pred);
```

Note that this would apply to all execution functions and all types of receivers if Ask 3 is accepted:
```
// Before (with Ask 3):
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


// After (with Ask 3):
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
```

- Bryce: Presumably, P1054-style helper functions would also be added to ease construction of receivers.
- Bryce: This would mean that `(bulk_)then_execute` could no longer be passed a regular `Callable` (a lambda, etc). When combined with Ask 3, this would mean ALL execution function swould no longer be passed a regular `Callable`.
- Bryce: I don't understand how this is needed for exception propagation. It just moves the burden out of the executor, to the receiver or the user of the executor.
- Lee: The error method should only be required if it's used.
- Kirk: I think it should always be required.
- Bryce: If I don't write an error method and call inline_executor::execute, should it fail to compile?
  - Lee: No
  - Kirk: Yes
- Jared: I could be okay with requiring this for then_execute.
- **Bryce: What I would want is to be able to `via` the value and error method separately. I want to be able to say `via(cpu).on_error(f), via(gpu).on_value(g)`.**
- Jared: That's what I want.
- Bryce: I need some before/after examples.
- **Bryce: Does error propagation in P0443 chain properly? If it's broken we need an example of how.**


**Ask 5:** Non-dependent execution functions should take `Callable`s that take an `Executor` parameter instead of nullary `Callable`s.

[EXECUTORS ISSUE #413](https://github.com/executors/executors/issues/413)

```
// Before:
struct VoidNoneReceiver /* AKA a `Callable` */ {
  void operator()();
};
struct OneWayExecutor {
  void execute(VoidNoneReceiver r);
};

exec.execute([] { /* ... */ });


// After:
struct ExecutorReceiver {
  void operator()(Executor exec);
};
struct OneWayExecutor {
  void execute(ExecutorReceiver r);
};

exec.execute([] (Executor e) { /* ... */ });
```

- Bryce: An executor is free to pass any `Executor` to a `ExecutorReceiver`, not necessarily themselves.
- Bryce: For example, in pushmi, the `std::thread` executor passes a "trampoline" executor that protects the stack. In the simplest case you can just pass an `inline_executor`.
- Jared: What's the motivation?
- Kirk: This is needed for lazy submission.
- Gordon: I'm concerned about whether this work.
- Jared: I need some examples.
- **Bryce: Kirk, Lee - Would you be fine with this being optional, not required?**


**Ask 6:** Receiver's value method shouldn't be the call operator.

[EXECUTORS ISSUE #414](https://github.com/executors/executors/issues/414)

```
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
```

Note that this change makes the most sense if done in conjunction with Ask 1 and Ask 4:
```
// Before (with Ask 1 and Ask 4):
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


// After (with Ask 1 and Ask 4):
// `execute` takes:
struct VoidNoneReceiver {
  // BOTH of these are required (NOT satisfied by `Callable`s):
  void done();
  void error(exception_arg_t, std::exception_ptr error);
};
// `twoway_execute` takes:
struct NoneReceiver {
  // BOTH of these are required (NOT satisfied by `Callable`s):
  U done();
  U error(exception_arg_t, std::exception_ptr error);
};
// `then_execute` takes:
struct Receiver {
  // BOTH of these are required (NOT satisfied by `Callable`s):
  U value(T value);
  U error(exception_arg_t, std::exception_ptr error);
};

// Using P1054-style `on_value`:
exec.execute(on_value([] { /* ... */ }));
auto f = exec.twoway_execute(on_value([] { /* ... */ }));
auto g = exec.then_execute(on_value([] { /* ... */ }), pred);
```

- Lee: This increases consistency.
- Jared: If both cases are required, I agree. Otherwise, I'd prefer the normal case to just be the call operator

#### (from Chris K) `then_execute` alternatives

- Bryce: You need two hooks, both to and from.
- Chris K: The other customization is done in .then.
- Bryce: At the start of this effort 2 years ago, we decided `then_execute` should be more primitive than `then`. I'm starting to feel like this was a mistake.
- Bryce: My thinking is we move towards a model where we have a single execution function no data dependencies.
- **Chris K and Bryce to sync up after meeting.**

### Action Items

- (Bryce) Create executor GitHub issues for the proposed `Receiver`-related changes.
- (Bryce, Chris K) Sync up after meeting regarding stream-based execution.


