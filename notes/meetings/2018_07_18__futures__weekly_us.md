ISO C++ Futures
===============

Weekly US Timezone Meeting - Notes - 2018-07-18
-----------------------------------------------

**Kindly forgive and identify any mistakes and omissions in these notes.**

Attending:
- Bryce Adelstein Lelbach (Note Taker)
- David Hollman
- Gordon Brown
- Kirk Shoop
- Lee Howes
- Mathias Stearn

Agenda:
- Debriefing from discussion of proposed `Receiver`-related changes to P0443 and P1054.
- Discuss issues stemming from the above and from email.
- Discuss parallel algorithms and executors interactions (David's emails).

Initial PR Plan ([repository, WIP](github.com/brycelelbach/executors)):
- 0) Add `None`/`SingleReceiver` concepts and replace raw callables/continuations with it
  - Status: Under discussion by executors group.
  - Futures Ask 0, [EXECUTORS ISSUE #408](https://github.com/executors/executors/issues/408)
  - Futures Ask 1, [EXECUTORS ISSUE #409](https://github.com/executors/executors/issues/409)
  - Futures Ask 2, [EXECUTORS ISSUE #410](https://github.com/executors/executors/issues/410)
  - Futures Ask 3, [EXECUTORS ISSUE #411](https://github.com/executors/executors/issues/411)
  - Futures Ask 4, [EXECUTORS ISSUE #412](https://github.com/executors/executors/issues/412)
  - Futures Ask 5, [EXECUTORS ISSUE #413](https://github.com/executors/executors/issues/413)
  - Futures Ask 6, [EXECUTORS ISSUE #414](https://github.com/executors/executors/issues/414)
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

David, Bryce: We should have a lexicon document.

#### Debriefing from discussion of proposed `Receiver`-related changes to P0443 and P1054

**Ask 1:** A receiver's error method should take a generic error type instead of an `std::exception_ptr`.

[EXECUTORS ISSUE #409](https://github.com/executors/executors/issues/409)

- Mathias: We may want an infallible future.
- Kirk: Haven't written up what the concept tree for something like this would look like. I don't think it's a good design.
- Mathias: Subsumption goes in the opposite direction than for `Receiver`s.
- David: How about having an `AlwaysFallible` and a `Fallible`, and then having futures be a composition of the two?
- Kirk: This differs from the default in C++.
- David: Default in C++ is to propagate errors up if they are uncaught.
- Kirk: You need this to be able to propagate.
- Gordon: Receiver needs to always be able to accept an error, but won't always receive one.


**Ask 4:** Receivers should be required to have both the value method and the error method.

[EXECUTORS ISSUE #412](https://github.com/executors/executors/issues/412)

- Bryce: Right now the default is to propagate errors if no error method is required.
- Bryce: Why should we require an error method instead of defaulting to propagate.
- Kirk: If you librarize your `Receiver`s, you probably have to deal with this in one place, and you probably had to anyways.
- Mathias: It's valid to pass a `FallibleReceiver` to an `InfallibleSender`, but not vice versa.
- Bryce: Do all `Executor`s, even those that do not care about errors, have to propagate today? Do we want that requirement? Or do we want it to fail to compile?
- Mathias: I think it should fail to compile.
- Gordon, Bryce: I agree.
- **Consensus: Try out Mathias' `Fallible`/`Infallible` approach.**


**Ask 5:** Non-dependent execution functions should take `Callable`s that take an `Executor` parameter instead of nullary `Callable`s.

[EXECUTORS ISSUE #413](https://github.com/executors/executors/issues/413)

- **Lee: Working on examples.**
- Bryce: Will add an action item for you.
- David: This may be needed for sub-executors.
- Bryce: One thing to note is that IIUC Jared's Agency passes through a descriptor of the shape of execution agents to its `Receiver`.
- Bryce: This would also allow asynchronous `Receiver`s to place requirements on their executor.
- Bryce: Maybe these would be spelled as two methods, but I'd imagine that calling `submit` on a `NoneReceiver` would execute it and calling `submit` on an `ExecutorReceiver` would enqueue work.
- Kirk: Work construction is done in the constructor of the sender.
- Bryce: Not sure /that/ fits my mental model, but will think about it.
- Bryce: New thread executor from pushmi.
- Kirk, Bryce: You get stuff out of futures by calling `submit`.

#### (from David) Generic parallel algorithms

- David: Started trying to write quicksort
- David: Need to be more specific about unwrapping.
- David: Only works if I have share?
- Bryce: Ah, the fundamental problem is you need to fork generically?
- Mathias, Bryce: Can't you do this with continuations? You receive the value in some continuation, then you fork there by calling one-way execute twice.
- Kirk: Maybe we should look at the alternative

Bryce: I need to make sure to share meeting notes, etc. Will be doing some logistics/project management stuff on the GitHub later today.

Bryce: We should have a resource list of gists, prototype implementations, etc.

### Open Questions (Not Addressed at This Meeting)

- Semantics of `via_execute`.
- Distinction between via and `via_execute`.
- `BulkReceiver` vs bulk operator.
- Implications of `Executor`s and `Future`s satisfying the same concepts.
- (from Gordon) Distinguishing between `Executor`s (which have properties) and `Future`s (which don't).

### Action Items

- (Bryce) Upload and organize meeting notes.
- (Bryce) Create executor GitHub issues for the proposed `Receiver`-related changes.
- (Bryce) Create lexicon document.
- (Bryce) Create resources document.
- (Mathias) Work on Ask #4.
- (Lee) Work on Ask #5.


