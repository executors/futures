# ISO C++ Futures

## Weekly Meeting - US Timezone - Notes - 2018-08-01

**Kindly forgive and identify any mistakes and omissions in these notes.**

#### Attendance

- Bryce Adelstein Lelbach (Note Taker)
- David Hollman
- Gordon Brown
- Kirk Shoop
- Lee Howes
- Mathias Stearn
- Michal Dominiak

#### Agenda

- Discuss David's new proposal and gist. 
- Explaination of lifters from reactive by Kirk.
- Bryce's update on the work queue model discussed at the 2018-07-27 executors meeting.

## Discussion

### David's New Proposal and Gist.

[David's Gist](https://gist.github.com/dhollman/0ff509a3a96a99f556d164c155e6b915)

*David begins presenting.*

*Starting with continuations.*

Mathias: In MyContinuation, what are R and S?

David: Lee indicated that he wanted to be able to return different types from the value and error methods.

Lee: Yes, but I don't think these functions should return anything, because they should be one-way.

David: This is intended to replace one-way and two-way.

Bryce, Lee: Right, but this continuation API is inherently two-way. The methods return something.

David: What are you proposing for future.then? Do you use a side channel?

Lee: For future.then, a receiver that gets passed the value is taken.

Bryce: I think that's what David would consider a side channel.

Mathias: I think there's a terminology issue. What traditional C++ futures call then is more like an adaptor in P1055. What's called a future in P1055 doesn't have an analog in traditional C++ futures.

*Discussion about which model is prevalent in other languages.*

Bryce: I agree side channels are needed in a P1055 model. But I don't think that's a problem.

Lee: P1053 was trying to show that you can build future.then on top of P1055

David: What you're saying is your passing the next receiver to value method?

Lee: No, it is passed in on construction.

Kirk: What we're trying to say is that when you only have `void`, the only thing that has to be passed through a side channel is the next continuation.

David: Even if these return `void`, you'd still need to template/pass through the `R` and `S` types.

Bryce: Why? I don't think it needs to be done at this layer. It would just be a part of the receiver's type.

Bryce: I think building futures on top of one-way continuations is perfectly fine.

Bryce: Regarding error handling, we should discuss that separately from the question of lazy vs eager. If they appear linked, we won't make any forward progress.

*Moving on to sub-executors.*

David: Kirk last week you indicated that you didn't want to pass receivers into receivers.

Kirk: I just think it's a little recursive, I'd prefer some sort of get method on the sender.

Lee: Kirk and I agree that propagating the executor/sub-executor down the chain. We don't like this particular design.

Bryce: By this particular design, you mean passing it as a parameter instead of having it be some component of the sender (which is retrieved by a get method).

Lee: Kirk's latest version of the gist does this. We're not disagreeing about the fundamental concept, just with the interface.

David: I like this approach because it fits better with bulk.

*Discussing error channels.*

Lee: There's a difference between handling errors from your asynchronous stream and handling errors from the executors.

Lee: I think it's feasible to separate those types of errors.

Bryce: How would that be done? Do you think it's fundamental that all errors go through the error method, or are two completely different paths needed?

Lee: If what you propagate is an async notion of what the value is, you can propagate an error, or a value that is both a value and an error.

Bryce: Who are you propagating to? To the next error method?

Lee: You are not propagating the async launch error to the next error method. But you will have a way to look at the propagated asynchronous error that happened.

Mathias: I think we have a different understanding of what a "switch to" executor is and when it happens.

Mathias: My mental model is that the enqueue happens when the previous value completes.

Bryce: It is fundamental that we support eager submission of dependent work in addition to what you just described Mathias. 

*Going back to David's gist.*

David: Two executors, one that is terminal (`MyExecutor`) and one that is not (`MyReusableExecutor`).

David: Bulk I've moved into a continuation. I think we just need to demonstrate that it doesn't have the same problems as the task factory approach.

Bryce: Why do you think it doesn't? I think the objection on Friday wasn't to task factories in particular. We looked at it, agreed the reformulation might work, but people didn't see the motivation for it.

Bryce, Lee: How would passing this bulk continuation to a non-bulk executor work?

David: If the bulk continuation is passed a non-bulk sub-executor, it just uses that.

Bryce: Ah, I see. You don't have to bind to a particular executor or disallow use with other executors as in the task factory model, because you always pass the sub executor in.

Bryce: I like this sub executor idea, and I like having it as a parameter as it makes it fundamental.

Lee: What is index in the bulk continuation?

David: It's a single index.

Bryce: How many execution agents does value call? Is it a bulk launch, or is it the element function?

David: It's the element function.

Lee: I think this is close to what I would like, except I would not expose new value and error methods. I would just make it satisify the non-bulk interface.

Bryce: I agree.

Bryce: When I was designing this, I imagined the continuation value method (that any executor can use) as being one that would handle the "default" case, contain the for loop, and spawn N execution agents. I had a separate element function method that bulk executors could use. Is what you are suggesting that the value method on a bulk continuation -is- the element function?

David, Lee: No.

Lee: If you made shape, *_factory, and *_element private, it would be fine; these don't need to be exposed. It could just be an implementation detail of the executor.

David: No, we need a common interface. I need to be able to pass a bulk task to an OpenMP executor, a CUDA executor, etc.

Lee: The things you pass this entity into do not need this interface.

Mathias, David, Bryce: I think we disagree.

Mathias: Does anyone have a use case for bulk tasks being used by non-bulk executors.

Bryce: Debugging. You might want to switch a bulk executor for a simple sequential executor, like the inline executor.

Mathias: I agree that something like an inline executor should be able to accept bulk tasks. But when would you ever want to call the value method on this continuation?

Lee, Bryce: It would be called by executors, when this gets wrapped up and passed down to that layer. 

David: Type-erased executor interfaces might need it to.

Bryce: It might be called when composing together execution graphs.

David: I think that's what Lee is saying.

Lee: I disagree that it would come up during type erasure. 

Mathias: That feels like it's not a bulk task, but instead it's a task that schedule works.

Lee: Any continuation you can use with futures is a thing that takes a value in and puts a value.

David: If you want to do non-fork-join bulk, like Mathias has described in the past, that wouldn't work.

Bryce: Lee, do you view all continuations, not just bulk continuations, as things that schedule work?

Bryce: We are starting to run low on time before the CppCon meeting. We have four weeks until the mailing deadline for the CppCon meeting, and six weeks until the meeting. I want everyone to start thinking about whether we can execute on a joint proposal by the CppCon meeting, or if we will be bringing separate papers/proposals to the CppCon meeting with the intention of working towards merging them there.

### Agenda Items Not Addressed at This Meeting

- Explaination of lifters from reactive by Kirk.
- Bryce's update on the work queue model discussed at the 2018-07-27 executors meeting.

### Open Questions (Not Addressed at This Meeting)

- Is it alright to build dependencies in side channels?
- Adaptor's use of `operator()`

