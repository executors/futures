ISO C++ Futures
===============

Biweekly Non-US Timezone Meeting - Notes - 2018-07-09
-----------------------------------------------------

Attendees:
- Bryce Adelstein Lelbach (Note Taker)
- David Hollman
- Eric Niebler
- Gordon Brown
- Kirk Shoop
- Michael Garland
- Michał Dominiak

### Discussion

How can P0443 and P1054 be modified based on P1055?
- Bryce:
  - First, `Promise`s, `Continuation`s, `Callable`s -> `Receiver`s.
  - Then, `ContinuableFuture`, `SharedFuture` -> `Sender`s.
  - Then, `Executors -> `Sender`s.

### Open Questions (Not Addressed at This Meeting)

- `Executors passing themselves to tasks.
- `Executor as `Future`s.
  - (from Gordon) How do you distinguish between executors (which have properties) and futures (which are lightweight things)?
