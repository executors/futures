ISO C++ Futures
===============

Pre 2018 Rapperswil Mailing Deadline Meeting - Notes - 2018-05-03
-----------------------------------------------------------------

**Kindly forgive and identify any mistakes and omissions in these notes.**

Attendees:
- Bryce Adelstein Lelbach (Note Taker)
- Lee Howes

### Synchronization

Forward progress:
- Blocking mechanisms need to call some `progress` mechanism for whatever executor they're running on.
- Don't hold locks when you call `progress`.

Issues:
- Pre-volta, scheduler was not fair, and could preempt you at any point, and you could be holding a mutex, and then be screwed.
- (Maybe this is a GPU problem, not an ISO problem): you can't call `try_lock` on a std::mutex on a GPU.
  - In the fiber example, you CAN actually call whatever OS synchronization mechanism, it just might deadlock you.
  - On a GPU, you may not be able to call it.
  - But it may not just be a GPU problem.
  - HPX mutexes/sync primitives can ONLY be called from HPX scheduler threads, because they context switch out (and thus assume the thread stack is an HPX managed one).

### P1054r0 AKA V1.0

By COB tmrw send draft to contributors.

Submit on Saturday evening.
- Plan is for Bryce to submit, but he is traveling. If Lee doesn't here from Bryce by Sunday, he should submit.

