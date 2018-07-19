ISO C++ Futures
===============

Weekly US Timezone Meeting - Notes - 2018-07-11
-----------------------------------------------

**Kindly forgive and identify any mistakes and omissions in these notes.**

Attending:
- Bryce Adelstein Lelbach (Note Taker)
- David Hollman
- Gordon Brown
- Kirk Shoop
- Lee Howes
- Mathias Stearn
- Michał Dominiak

Agenda:
- Review high-level strategy for initial PR against P0443 based on P1053/1054/1055 for Friday.
- Discuss issues stemming from the above and from email.

Initial PR Plan ([repository, WIP](github.com/brycelelbach/executors)):
- 0) Add `None`/`SingleReceiver` concepts and replace raw callables/continuations with them.
  - Status: In progress.
- 1) Add P1054 style `Receiver` wrappers.
  - Status: TODO.
- 2) Add `None`/`SingleSender` concepts.
  - Status: TODO.
- 3) Replace `then_execute` with `via_execute` or an alternative.
  - (as member function or property, transition to a customization point later).
  - Status: TODO.
- 4) Refactor two-way execution in terms of one-way execution.
  - Status: TODO.
- 5) Refactor bulk execution in terms of one-way execution.
  - Status: TODO.
- 6) Replace executor concepts with sender concepts.
  - Status: TODO.

### Discussion 

(from David) Using P0443 friendly terminology
- Bryce: I was planning to try and do that.
- Bryce, Kirk: Concerned about overloading terminology.

Proposed `Reciever`-related proposed changes to P0443 and P1054:
- `NoneReceiver::done` vs `SingleReceiver::done`.
  - Michal: Remove `done` from `SingleReceiver`, you can require both concepts if you need to.
  - Kirk: `done` is needed `ManyReceiver`, which has the same interface as `SingleReceiver`, but can receive multiple values/errors.
  - **Consensus: No `done` for now.**
- `Callable`-compatible `Receiver`s.
  - Consensus: `Callable` compatibility isn't critical at the `Executor` interface level.
`Executor`s passing themselves to Receivers
  - Kirk: The `Receiver`'s value method runs in the caller's context, and submits the actual work (which doesn't need to take the `Executor`) to the `Executor`.
  - Bryce: Ah, I think I understand now, will have to think about this. Passing the `Executor` may be useful for bulk `Receiver`s.

Bulk Execution:
- (from Mathias) Deferring completion during bulk execution
- (from Kirk) Data movement and bulk execution
  - Bryce: Bulk is not tied to this. SG1 decided to only deal with accessible memory for now. There are natively-bulk execution frameworks that do not need data movement because they have global memory.
  - Bryce, Gordon: Not a part of executors. Data movement is something to be addressed separately.

### Open Questions (Not Addressed at This Meeting)

- Semantics of `via_execute`.
- Distinction between via and `via_execute`.
- `BulkReceiver` vs bulk operator.
- Implications of `Executor`s and `Future`s satisfying the same concepts.
- (from Gordon) Distinguishing between `Executor`s (which have properties) and `Future`s (which don't).

