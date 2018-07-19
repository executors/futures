Version 1 Outline
=================

* Motivation
  * What problem are we solving?
    * Avoid detailed discussion in this paper.
      * Instead, reference prior papers co-authored by Bryce, Lee and others.
  * Design: How are we solving the problem?
    * Avoid detailed discussion of all the alternative design options.
      * Instead, reference prior papers co-authored by Bryce, Lee and others.
    * Focus on the design decisions we have decided on. 
      * Reference supporting polls/minutes from previous committee meetings to support our decisions. 
  * Examples (we won't be able to do all of these, but we should try to get some volunteers to work on these)
    * Archetypes
      * Always ready future
      * Locking traditional future
      * Lockfree traditional future
      * CUDA future
      * HPX/fiber future
      * Network/distributed future
    * Heterogeneous future interop
      * E.g. `network_future(/* ... */).via(cuda_executor).then(f).via(hpx_executor).then(g).via(network_executor)`
    * Coroutines TS future interop
    * Generic executor-based `async` (not part of this proposal, but could be an example in the repo)
    * Generic `when_*` (not part of this proposal, but could be an example in the repo)
    * Generic asynchronous algorithm (not part of this proposal, but could be an example in the repo)
    * Custom future_wait/future_get function
  * Future Work (things that are not version 1 features)
    * `std2::async`
    * `std2::when_*`
    * Cancellation
    * `.bulk_then`
* Wording
  * Promise concept
  * Executor promise property
  * Error-handling disambiguation tag
  * Future concepts
    * SemiFuture
    * ContinuableFuture
    * UniqueFuture [OPEN TECHNICAL QUESTION, ISSUE #26 AND #27]
    * SharedFuture [OPEN TECHNICAL QUESTION, ISSUE #26 AND #27]
  * .then
    * Syntax
    * Semantics
    * Requirements [ISSUE #25 AND #27]
  * .via
    * Syntax
    * Semantics
    * Requirements [ISSUE #21]
  * Requirements on continuation functions 
  * Error handling model
  * Future unwrapping constructor [OPEN TECHNICAL QUESTION, ISSUE #29]
  * Forward progress guarantees [OPEN TECHNICAL QUESTION, ISSUE #6]
  * Future/promise synchronization gurantees [OPEN TECHNICAL QUESTION, ISSUES #2 AND #3]
    * is_always_lockfree(ish) property
  * Concrete executor type-erasing future types
    * std2::semi_future
    * std2::continuable_future
  * std::thread Future Waiting Functions
    * std::this_thread::future_wait (ADLable)
    * std::this_thread::future_get (ADLable)

