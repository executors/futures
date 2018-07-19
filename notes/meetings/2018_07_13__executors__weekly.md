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

#### Review of proposed `Receiver`-related changes to P0443 and P1054:

Ask #0

```
TODO
```

- Bryce: Places additional burden on authors of Receiver.
- Jared: Would you be able to just write an overload for one concrete type?
- Bryce: This is related to whether the error method should be required.
- Two options if no error method matches:
  - Compile fail.
  - Propagate.
- Chris K: Impact on polymorphic?


Ask #1

```
TODO
```

- Consensus to do this.


Ask #2

```
TODO
```

- Jared: then_execute(on_error(), f) -> future<?>
- Bryce: Maybe future<void>
- Lee: I think so
- Jared: More motivation/use cases
- Will come back with examples


Ask #3

```
TODO
```

- Bryce: P0443 is consistent with the IS; exceptions that escape from a thread of execution lead to std::terminate
- Lee: The example is a non-blocking launch + a shutdown request that arrives before the work is started
- Chris K: This should be handled through a different channel
- Chris K: The problem is that the error function is not called in the right execution agent. But for then_execute, it will be called in the execution agent. You are using the same channel for two different things.
- Bryce: For then_execute, can this be called due to a launch error.
- Bryce: The proposal is that this error method would be used to handle both "system" errors and errors contained in tasks?
- Bryce: This means that error is invoked on an unspecified execution agent. Remember the Concurrency TS v1
- Jared: Is an executor allowed to throw for synchronous errors? Or does it have to use the error method?
- Bryce: If you don't always use the error method, changes in the implementation of an executor can change the error handling path. I think the error method would always have to be used.
- Lee: Not just about launch errors, also things like shutdown, migration, cancellation, etc.
- Chris K: We want two channels, one for dependency errors, one for system.
- Lee: What if the preceding task forwarded the error to it's dependency?
- Idea: Have two channels (or a way to signal error category) on Receivers
- Bryce: Two Qs for Jared and Chris K:
- Should we provide some way of providing an error handling hook for system errors?
- Would you feel differently about this if there was a way to distinguish the error category (system or predecessor)?
- Jared: Could this be done with properties?
- Lee + Kirk: Executors do not have to report system errors with the error method even if it exists.
- Lee + Kirk: This is needed for error propagation if we rebase then_execute on execute.
- Bryce: That makes error handling inconsistency in generic code.

Ask #4

```
TODO
```

- Lee: Should only be required if it's used.
- Kirk: Should always be required.
- Bryce: if I don't write an error method and call inline_executor::execute, should it fail to compile?
- Lee: No
- Kirk: Yes
- Jared: I could be okay with requiring this for then_execute.
- Bryce: What I would want is to be able to via() the value and error method separately. I want to be able to say via(cpu).on_error(f), via(gpu).on_value(g)
- Jared: That's what I want.
- Bryce: I need some before after examples
- Open Q: Does error propagation in P0443 chain properly? Need an example.

Ask #5

```
TODO
```

- Jared: What's the motivation?
- Gordon: Concerned about whether this work.
- Open Q: Kirk + Lee: Would you be fine with this being optional, not required?

Ask #6

```
TODO
```

- Lee: Increases consistency.
- Jared: If both cases are required, I agree. Otherwise, I'd prefer the normal case to just be the call operator

#### (from Chris K) `then_execute` alternatives

- Bryce: You need two hooks, both to and from.
- Chris K: The other customization is done in .then.
- Bryce: At the start of this effort 2 years ago, we decided `then_execute` should be more primitive than `then`. I'm starting to feel like this was a mistake.
- Bryce: My thinking is we move towards a model where we have a single execution function no data dependencies.
- Chris K and Bryce to sync up after meeting.

### Action Items

- (Bryce) Create executor GitHub issues for the proposed `Receiver`-related changes.
- (Bryce, Chris K) Sync up after meeting regarding stream-based execution.


