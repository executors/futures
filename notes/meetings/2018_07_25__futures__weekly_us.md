ISO C++ Futures
===============

Weekly US Timezone Meeting - Notes - 2018-07-25
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
- Discuss task factory model and issues relating to it.

### Discussion

David: `Adaptor` is using the call operator, perhaps it should be called
something else.

Bryce: I don't see `Adaptor` as fundamental.

David: It's as fundamental as `future::then`.

Bryce: I don't see that as fundamental.

*David, Kirk try to explain `Adaptor` to Bryce.*

Gordon: Not clear how you pass through an `Executor` through an entire execution chain. If I have to pass through both a result value and an executor, what do you do? I tried just sending through a pair, but it didn't seem clean.

Kirk: Two ways to do this in pushmi. What you described, and something like the trampoline executor.

Gordon: Concerned that the model may embed an implicit barrier. What if you want to do task fusion?

Lee: But if you're going to wait for the predecessor value before you launch, then you need it anyways.

Gordon: If the design is intended to pass the executor through to the receiver, what are we supposed to do?

David: For example, if you're using a thread pool executor, the application code needs to know what thread it's executing on.

Kirk: You can do that with thread local storage.

Bryce, David: No, not all executors invoke new threads.

Bryce: What about a GPU executor? Or an HPX executor?

Lee: All C++ code runs in some thread; thus all executors run in some thread.

Bryce: Ah, yes. I see what you're saying.

David: It's the moral equivalent of setting a TLS variable, then calling a function that reads it, then reseting it afterwards, instead of changing the signature of the function.

David: How does this work for generic code?

Kirk: The trampoline executor is just one concrete example, the fact that it uses TLS is an implementation detail. You could write your own that uses it's own TLS.

Mathias: If you have multiple TLS varibles, how do you know all the ones that you have to update?

Bryce: Trampoline is just used for a deferred submission model. It's not necessarily universal.

David, Mathias: Not clear how this works for nested executors or thread pool executors.

Bryce: There is a distinction between a Receiver that expects an Executor (and thus does an asynchronous launch) and a Receiver that expects a value (e.g. dependent execution). Gordon is saying you might need both an Executor and a value in the case of asynchronous dependent execution (which is not something that pushmi normally does).

Bryce: This makes me think there may be a dichotomy here, and perhaps we should explicitly have different interfaces for asynchronous launch (e.g. executors) and dependent execution (e.g. futures).

David: How about adding a Sender argument to set_value and set_error, indicating which executor to run on it. Kirk, you indicated this was problematic. Why?

Kirk: There's nothing that prescribes that the Sender you're passing in is an executor, which would make this weird.

Bryce: I think Kirk finds this odd because futures and executors are the same thing in the pushmi model, so you can't really add an "executor" only parameter.

Gordon: Right, but what if you need to support an eager submission model? In that case, you need this. If we can't do that in the current model, then maybe executors shouldn't be futures.

David, Mathias: Maybe executors should be things that make Senders.

Bryce: So maybe executors should be Adaptors?

Mathias: No, you need something different. Adaptors are like then_execute, they take a sender. The fundamental thing is something that is one way.

Gordon: This would sort of look like the "executors as task factories" model, similar to Bryce's PR against P0443.

David: Are tasks receivers? Or senders?

Bryce: Right now, we're thinking of tasks as receivers.

Kirk: They could also be senders.

Bryce: We could think of tasks as another, third thing, which can be represented as a receiver or as a sender.

Bryce: E.g. you can have a receiver that models/contains a task.

David: Can we define a new concept for executor?

Bryce: Something like Adaptor, with a method `execute` that returns a Sender and takes... a Receiver? Nothing?

Mathias: I think it would have to be nullary.

Kirk: I imagine adding some mechanism to a sender that would let you get a task factory from it.

Kirk: A nullary method on senders that returns an ExecutorFactory that has a method that makes a sender.

Mathias: https://gist.github.com/RedBeard0531/1fb12c3bd9c469a8549bab6c9e22492b#file-lazy_futures-h-L430-L476

Bryce: That looks a lot like `make_promise_contract`.

Mathias: It is.

Kirk: What happens if you attach a continuation afterwards?

Kirk: `Lifter`s - functions that take receivers and return receivers. Not in pushmi yet.

Mathias: This model pushes at the end.

Mathias: The `promise<void>` is only produced by rvalue-reference qualified methods.

Kirk: What does completion/continuation attachment?

Mathias: `get`.

Mathias: I found that when working with multiple future types, I needed type erasure.

Kirk: Showed how you can avoid this in an email thread.

### Open Questions (Not Addressed at This Meeting)

- Semantics of `via_execute`.
- Distinction between via and `via_execute`.
- `BulkReceiver` vs bulk operator.
- Implications of `Executor`s and `Future`s satisfying the same concepts.
- (from Gordon) Distinguishing between `Executor`s (which have properties) and `Future`s (which don't).

### Action Items

- (Bryce) Process other action items from these notes.


