ISO C++ Futures
===============

Pre Face-to-Face 2018-04-26 at NVIDIA - Notes - 2018-04-25
----------------------------------------------------------

**Kindly forgive and identify any mistakes and omissions in these minutes.**

Attendees:
- Bryce Adelstein Lelbach (Note Taker)
- Lee Howes

Discussed the following things not relating to the face to face meeting at NVIDIA tomorrow:
- Implementing future/promise model in CUDA.
  - Bryce: I think it can be done.
- Incorporating executor promise property paper into this paper.
  - Bryce: I wants to have the concepts and mechanism in this paper so it's self contained.
  - Lee: I'm oncerned about this paper growing too large.
  - Consensus: We're okay with doing this, but want to wait to hear back from Chris (which one?) next week on the design.
- Subsetting `then_execute` out of the executors proposal.
  - Lee: I feel this might be necessary for him and others (ex: LEWG) to be comfortable with this.
  - Bryce: I feel we should have done this a long time ago and would be okay with this.
  - Bryce: I think that we shouldn't push for this now as LEWG asked us to limit changes to the executors paper.
  - Bryce: But we should explore this if it becomes clear it would increase consensus during/after Rapperswil.

Action items before the face to face meeting at NVIDIA tomorrow:
- Add separate outline document to repo.
- Add placeholder sections to the paper.
- Review and triage Github issues.

Planning for the face to face meeting at NVIDIA tomorrow:
- Agenda:
  - Discuss and formulate solutions to remaining open technical questions that need answers for the Rapperswil paper.
  - Review and expand on paper outline.
  - Identify action items in both the paper and the reference implementation.
  - Assign action items to collaborators.
  - Review and triage Github issues.
- We should try to keep the group on topic and avoid tangents.
- Some attendees unfamiliar with ISO process/new to participation (Felix, Sean).
- Some attendees new to this effort (Thomas Rodgers).

Outline:
- Motivation
  - What problem are we solving?
    - Avoid detailed discussion in this paper.
      - Instead, reference prior papers co-authored by Bryce, Lee and others.
  - Design: How are we solving the problem?
    - Avoid detailed discussion of all the alternative design options.
      - Instead, reference prior papers co-authored by Bryce, Lee and others.
    - Focus on the design decisions we have decided on. 
      - Reference supporting polls/minutes from previous committee meetings to support our decisions. 
  - Examples (we won't be able to do all of these, but we should try to get some volunteers to work on these)
    - Archetypes
      - Always ready future
      - Locking traditional future
      - Lockfree traditional future
      - CUDA future
      - HPX/fiber future
      - Network/distributed future
    - Heterogeneous future interop
        E.g. `network_future(/* ... */).via(cuda_executor).then(f).via(hpx_executor).then(g).via(network_executor)`.
    - Coroutines TS future interop
    - Generic executor-based `async` (not part of this proposal, but could be an example in the repo)
    - Generic `when_*` (not part of this proposal, but could be an example in the repo)
    - Generic asynchronous algorithm (not part of this proposal, but could be an example in the repo)
    - Custom `future_wait`/`future_get` function
  - Future Work (things that are not version 1 features)
    - `std2::async`
    - `std2::when_*`
    - Cancellation
    - `.bulk_then`
- Wording
  - Promise concept
  - Executor promise property
  - Error-handling disambiguation tag
  - Future concepts
    - SemiFuture
    - ContinuableFuture
    - UniqueFuture [OPEN TECHNICAL QUESTION, ISSUE #26 AND #27]
    - SharedFuture [OPEN TECHNICAL QUESTION, ISSUE #26 AND #27]
  - `.then`
    - Syntax
    - Semantics
    - Requirements [ISSUE #25 AND #27]
  - .via
    - Syntax
    - Semantics
    - Requirements [ISSUE #21]
  - Requirements on continuation functions 
  - Error handling model
  - Future unwrapping constructor [OPEN TECHNICAL QUESTION, ISSUE #29]
  - Forward progress guarantees [OPEN TECHNICAL QUESTION, ISSUE #6]
  - Future/promise synchronization gurantees [OPEN TECHNICAL QUESTION, ISSUES #2 AND #3]
    - `is_always_lockfree(ish)` property
  - Concrete executor type-erasing future types
    - std2::semi_future
    - std2::continuable_future
  - std::thread Future Waiting Functions
    - std::this_thread::future_wait (ADLable)
    - std::this_thread::future_get (ADLable)

New questions from call (mostly notes to myself of stuff to look at):
- Look at `.via`/`.on` consistency discussions [ISSUE #4]
- Does `hpx_future<gpu_future<int>>` unwrap? [ISSUE #29]
- Are nested CUDA streams a thing (in CNP for example)?
- Is future<T> convertible to future<void> (HPX does this)?
- Is every unique future type convertible to a shared future type?

