# ISO C++ Executors

## Weekly Meeting - Notes - 2018-07-27

**Kindly forgive and identify any mistakes and omissions in these notes.**

#### Attendance

- Bryce Adelstein Lelbach (Note Taker)
- Carter Edwards
- Chris Kohlhoff
- David Hollman
- Gordon Brown
- Jared Hoberock
- Kirk Shoop
- Lee Howes
- Michael Garland

#### Agenda

- Continue discussion of P0443/P1055 unification.
  - Discuss task factory model.
- Review execution context [EXECUTOR PR #402](https://github.com/executors/executors/pull/402).

#### Summary

This week, we discussed moving towards a task factory model based on prototype work that Chris K worked on over the last week.

After discussion, there seemed to be a consensus that we do not want to move in the direction of task factories.
We discussed an alternative work-queue model that would let us decouple futures and data dependencies to a degree, replacing them with a coarser grained dependency mechanism.
Bryce and Chris K will continue working on this and we will revisit it next week.

We also discussed Carter's outstanding [EXECUTOR PR #402](https://github.com/executors/executors/pull/402) on execution contexts.

### Discussion

#### P0443/P1055 Unification

Jared: Last week we asked Chris K to go prototype task factories. Let's start with that.

*Looked at Chris K's branch in the executors-impl repo*

Bryce: Would we still have `then_execute`?

Chris K: It would become a property. We'd just have `execute` as an execution function.

Chris K, David: Adaption is hard if there's an arbitrarily large set of properties.

Gordon, Bryce: In our prototypes we had concrete types, e.g. `bulk_task`, to simplify this.

Jared: Is this better? What does this get us?

Chris K, Mathias: Not clear that it is.

Gordon: This may give us a path to enable lazy execution.

David: Does this give Kirk what he needs?

Kirk: No, it doesn't.

Chris K: This is the properties equivalent of pushmi's customization points.

David: Kirk, why doesn't this help you?

Kirk: It doesn't make the default interface handle errors, cancellation, or be extensible to concepts, etc

Chris K: That's orthogonal. If you want to add error handling, cancellation, etc, you'd do it with a property.

Kirk: I only added customization points for bulk, this isn't how I'd build the core.

Bryce: How would you transition dependencies between executors without customization points?

Kirk: Yes, that needs it too. pushmi doesn't need that, so it's not fundamental.

David: But bulk and inter-executor dependencies are fundamental to other people.

David: You added what others needed via customization points, but you're saying that adding the functionality you need to P0443 with properties doesn't work. Why?

Kirk: I haven't seen anyone show me what I need with this model.

Gordon: I think this can work and support what pushmi requires.

Bryce: Kirk, would an example you help you?

Lee: Bryce, does this meet your needs?

Bryce: I think this would work for bulk. If you passed a non-device function to a non-polymorphic executor, it would fail at compile time with CUDA today. With a polymorphic executor, it would fail at runtime.

Bryce: But I don't think we should think about things in terms of the limitations of a particular GPU programming framework today. That's just an implementation detail. We should just focus on expressing bulk in standard C++.

Jared: Is this an improvement? I don't think so. It makes bulk execution second class.

Bryce: By logic, it also makes two-way and dependent execution (then) second class.

Kirk: What if we still had require and query, and just a single interface method?

Mathias: That makes executors a very general purpose thing, they'd just be a bag of things.

Kirk: That's what they are today. Example: properties for getting thread ids, for getting future/promises, etc.

Mathias: But there's a fundamental interface.

Bryce: I'm not sure how far you can get with the base operations without properties if you're doing two-way or dependent execution. You need `make_promise_contract` pretty quickly.

Mathias: Properties shouldn't be things that are fundamental. If `make_promise_contract` is fundamental, then it shouldn't be a property.

David: But different people believe different things are fundamental. Facebook doesn't believe that bulk is fundamental.

Kirk: I disagree, I (personally) feel that bulk doesn't fit, not that it's not fundamental.

Lee: We care about bulk.

Bryce: Where do we go from here?

Jared: Chris what do you think? You implemented it?

Chris K: I don't think this is the right model for fundamental execution functions, like `execute` and `bulk_execute`. You can make it fit, but it doesn't fit cleanly. I'd rather keep `bulk_execute` as a function.

Gordon: One thing about this approach is that executors are no longer objects that execute work, they're factories that create tasks.

Michael: I don't even understand why these classes have execute anymore, if their whole purpose is to create things tasks.

Bryce: I don't think we all have the same definition of "executor". I don't think the P0443 and pushmi definitions are necessarily the same thing or compatible. I'm not sure that's a bad thing.

Gordon: I agree.

Gordon: We discussed this on the futures call on 2018-07-25 - having executors be senders as well is still problematic for me. It means that executors are passed down the chain of tasks.

Kirk: I think you can do the same things with a pushmi executor that you can do with a P0443 executor. The existing P0443 model doesn't really support composition while keeping the executor.

Gordon: You can compose in P0443 with `then_execute`.

Gordon: What would we have to add to this design to support pushmi?

Bryce: By "this design" do you mean the proposed changes from Chris K, or P0443?

Gordon: The proposed changes from Chris K.

Jared, Michael: I think we're saying we don't want to go in this direction, though.

Mathias: Is the consensus that we don't want to go in this direction?

David: We tried this as a way to move towards a consensus proposal. This seems to have not succeeded.

Bryce: We've spent a lot of time exploring reducing the number of execution functions in P0443. Are we now at a point where we no longer want to explore that route?

Jared: There are other ways to approach this that might work.

Mathias: I have an idea for an alternative futures model that may support lazy execution.

Kirk: This sounds like super-type.

Mathias: I disagree.

Mathias: I want futures to be usable, not a minimal concept.

Mathias: We shouldn't focus on making it easy to write futures. The burden should be on the authors, not the users.

Kirk: I somewhat agree, but there are many different types of futures that people want to write.

Jared, Bryce: I think we should move towards a model where futures are decoupled from P0443.

Lee: I agree, but we still need a way to do dependent execution.

Jared: Can we do that in the next month?

Mathias: I'd be okay with trying to come up with a minimal future concept.

Bryce: That's what we did. I think what Jared is saying is we should move towards a futureless P0443 that has execution dependencies, but not data dependencies.

Lee: We still need a way to manage data lifetimes.

Mathias: I think the terminal forms are important.

Jared: Is there anyone who thinks we DON'T need some sort of task chaining mechanism for September?

David: No.

Mathias: I think I could be okay with that.

Lee: I think it's a question of greedy task chaining.

Bryce: I think I could build on top of sequenced executors (e.g. work queues) for some models, but not for everything. I'm not sure how I'd do it with HPX, for example.

Bryce: I could see us splitting into two proposals - execution and dependent execution - but I'm not sure that I would want to have the first of those make it into the standard without the second.

David: I'm concerned about relying on data dependencies as a side channel.

Michael: I agree with Bryce, I could see how you could build things on top of work queues. But, you loose some capabilities, like point to point dependencies. You end up with something coarser.

Mathias: Ordering would be more dependent on scheduling. 

David: Ordering doesn't give you concurrency.

Michael, Bryce: I disagree. You can create concurrency by creating more executors.

Michael: If you had no way to create point to point dependencies with `then_execute`, you'd end up with a model where you'd have ordered executors that would guarantee that work enqueued to them would run in order.

Michael: This is kinda related to something Torvald said a few meetings ago, when he asked how you would wait on all outstanding work.

David: How would you do a join in this model?

Lee: The question is whether we would need both initially.

Kirk: This work queuing model is what we do in reactive. It's the many model.

Michael: What's the many model? It replaces the value method with a next method that can be called multiple times. It's the closest thing to an async iterator.

Bryce: Do we want to look at a work queuing model?

Jared: We spent some time looking at this last week, and we ran into some models, which is why we moved towards this model.

Michael: Is a meta-point that one of the biggest points of contention is the use of futures in P0443?

Kirk: That is one way to express it.

Kirk: I would express it as: why aren't P0443 executors futures?

Michael: The biggest difference is that futures are inherently one use, and executors are not.

Kirk: I don't think I agree that futures are one use.

Mathias: We discussed on the 2018-07-25 futures call a small change - making executors have a nullary function that returns a sender (e.g. making executors some sort of factory).

Bryce: Isn't that similar to the task factories idea we just discussed?

Michael: One naive fear I have about thinking of executors as futures is then you might end up needing a "shared" executor.

Kirk: The difference between a future and shared future in the pushmi model is whether or not submit is rvalue ref qualified.

Michael: What you're saying is that the model of futures in pushmi doesn't suffer from the unique future/shared future distinction in the P0443/P1054 model.

Jared: Bryce and Chris please come back next week with a proposal for coarser grained dependencies/work queue model.

#### Execution Context EXECUTORS PR #402

Jared: Moving on to Carter's pull request.

*Wording review of [EXECUTORS PR #402](https://github.com/executors/executors/pull/402)*

### Action Items

- (Bryce, Chris) Explore and prepare proposal on work queue model for next week's call.
- (Carter) Update [EXECUTORS PR #402](https://github.com/executors/executors/pull/402).

