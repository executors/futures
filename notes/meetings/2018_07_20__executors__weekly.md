# ISO C++ Executors

## Weekly Meeting - Notes - 2018-07-27

**Kindly forgive and identify any mistakes and omissions in these notes.**

#### Attendance

- Jared Hoberock (Note Taker)
- Chris Kohlhoff
- Kirk Shoop
- David Hollman
- Carter Edwards
- Lee Howes

#### Agenda

- Continue discussion of P0443/P1055 unification.
  - Discuss `DependencyId` model (alternative to futures).
- Review execution context [EXECUTOR PR #402](https://github.com/executors/executors/pull/402).

#### Summary

Today we discussed two different ideas for reducing the number of execution functions.

- Jared presented a prototype of ChrisK's idea of introducing DependencyIds, intended to be a property-based, primitive substitute for full-fledged Future types.
- Lee presented a prototype built on top of pushmi which exposed different task
    types via customization points.

There was more support for Lee's direction if it could be expressed via P0443's existing properties mechanism.
We will follow up on this direction next week.
The hope is that it will allow P0443 to define a single .execute() function and allow different types of tasks to be introduced via the properties extension mechanism rather than through different executor member functions.

We also discussed Carter's outstanding [EXECUTOR PR #402](https://github.com/executors/executors/pull/402) on execution contexts.

### Discussion

#### P0443/P1055 Unification

Jared: *presents dependency id experiment*

Kirk: Querying for an id is racy

Jared: Ok, noted

Chris K: If the property is not present, then it doesn't actually affect semantics

Lee: Building dependencies is required? if you can't enforce an ordering, you can't relax

Chris K: We've been told that the lazy future stuff can be built on top of oneway

Chris K: you'd do lazy chaining in the higher level lib. the problem we're trying to solve: Cuda has dependency support in the runtime

Chris K: We're trying to provide a way to communicate through to the runtime

Chris K: The way i'd expect it to work is that it's not required to be present to work

Chris K: So you could still write a lazy-based chaining library

Lee: That's true. allows you to greedily chain into dependencies

Chris K: This does allow some easure of lazy as well. require still has an opportunity to xfrm the type

DavidD: But query doesn't, right? that's one concern i'd have. if you query for the token you can't put a non type erased continuation

Chris K: I'd have to have a look at the example programs. i'm not sure if there's a need to query for an ID or not

Chris K: If we have a separate signalling factory, it may be that that's not required

David: You'd still need to

David: The signaler factory needs to take a continuation to run to do the type erasure

David: You have to generate the token in the same statement as when you submit the continuation.

Gordon: This is why i was thinking .execute() could return the id itself. you could have execute return a continuation that could be passed as a contnuation to another executor

Chris K: Just an idea -- not intended to be necessary

Chris K: I'll go away and experiment with it. this is predominantly about supporting the runtime dependency stuff that runtimes have indepdently of high evel chaining libs

Matthias: What is the difference b/t a dependency id and a future with no public methods

David: By no public method, i think you mean no concept enforced methods

Kirk: Something that seems to be lost here - i dont know how to characterize it, could be a loss of fwd progress if signal never gets called

Kirk: The thing that used the depends_on is just blocked forever

Mathias: Isn't that also a problem with P1055 receivers? if you don't call it you lose fwd prog

Kirk: The difference here is that you dont start the work until you give a receiver. in this case, you do start the work and you wait for the receiver

Jared: What do you mean by start and then wait?

Kirk: This allows deadlocks without calling signal. in P1055 there's no deadlock unless you call the receiver

Mathias: Calling the .signal() method to me ffels exactly isomorphic to .value() or .done() or whatever on a Receiver

Mathias: The fact that it is possible to forget to call it, feels concerning to me as well but it seems

Chris K: This not aimed at end users so much

David: That's entirely the same thing bc the P0443 proposal targets a slightly lower level than the P1055 proposal. so i'd say the logic applies to both cases

David: I dont think most users should call either of these methods

Kirk: Just pointing out that we'd lose the guarantee from P0443, it could be reintroduced at a higher level

Mathias: It could also be guaranteed via a dtor of a signaller

Kirk: Dtor signalling is a real problem

David: You can't do things youre not supposed to do by boost blocking

Kirk: You can only use TLS to communicate state

Mathias: Signal() has no args, that's already the case

Kirk: There's a difference between a method called .signal() and a dtor

David: You can trampoline

Kirk: The only way to trampoline is to queue something somewhere, and i wouldn't want to queue from a dtor. the dtor is not a place to do work, it's a place to release resources

Mathias: I know a lot of algos build on folly future do work in dtors

Kirk: I cant defend that

Lee: I cant think of any examples off the top of my head

Mathias: I'll find a link

Jared: Do people think this is at least worth exploring?

David: It's pretty close to a NoneReceiver.

Kirk: NoneSender as .submit() method with a NoneReceiver. this is nothing like that

David: If you really stripped it way down, it has analogies to promise_factory or promise_contract ideas from earlier

David: I understand some of the particulars are different. it'd be helpful to know how the particulars differ

David: I want to understand those tradeoffs. that's my own homework

Lee: As a mechanism for avoiding Futures in this paper, i think this is a reasonable starting point for discussion. it will need a lot of clarity on how we really expect to efficiently pass values & errors around

Lee: Would we always hang this stuff off the side in shared state? everything else is sidebanded? that sounds like it could become expensive in practice

Gordon: From this as a starting point, we could make these ids richer types

Lee: At which point you're approaching Future. or should we just close the gap b/t executors & futures

Gordon: on a related note: Is it correct to draw an analogy between the task and the receiver in P1055?

Kirk: I dont see the connection

Gordon: So .signal() would be like .set_value() or .done() on a reciever?

Kirk: If you think of .signal() as .done(), it does map task_a to being a subset of a NoneReceiver that only has .done() on it

David: That's what i was trying to get at. what is lost there? and what are we losing functionally by losing those kinds of pieces?

Mathias: One thing i noticed that is different. these are only able to handle the equivalent of a future<void> they are not able to do a future<T>

Mathias: They're not able to propagate data. that leaves the user to do it another way, which we've found problematic

Chris K: That's the intention, that you would build up futures on top of it. inside of ctl structures, you'd use these properties to communicate info through to the executor

Chris K: It's not intended to give you semantics that you can universally rely on.

Chris K: I would expect you to go away and write Futures that would still work just fine if you were never given an executor that could support these

Lee: If all executors can handle is ordering of tasks and can't optimize the way data flows between then, that's a limited set of capabilities

Lee: Id question whether we really get much value out of that in the end

Lee: It covers that limited case where you use these tokens to build a work queue and allocation happens out of band

Lee: But you lose the ability of an executor to participate in optimization

Lee: Youd have to question whether the executor itself adds much value there at all

Chris K: Jared's written some examples, we dont have an answer right now. taking things like a really lightweight future chaining library

Lee: That's fair, theres nothing wrong with exploring this. maybe you can show that there's significant

Jared: The whole idea of this proposal is to eliminate functionality -- the extra executino functions

Lee: Can i run you through some example code that would take a different tack?

Lee: This is in issue #417

Lee: What i am getting at here -- without chaning the concepts in pushmi, we just make some small changes so that rather than special casing bulk, we dont necessarily special case bulk for asynchrony

Lee: The way we can achieve this on top of pushmi -- pushmi passes values through as each task completes

Lee: That's what futures do traditionally. what you really need to do for gpus - when you chain using futures, you need to hook inside and ask what is the sync prim i actually need here

Lee: The actual value is going to be needed at the end

Lee: This is a trivial task that adds 1, it

Lee: This code is going to run ahead as scalar code, it's not going to wait for the actual code to complete. it'll convert this code into a token

Lee: *continues describing code example*

Lee: Rather than trying to encode dependency magic in the executor, we can use the executor as the paramterization hook in customization points outside the executor

Lee: We'd just define all sorts of free functions. we dont need to hook up magic on the executor

Gordon: Can we see async_fork to see how it works

Lee: *describes async_fork impl*

Gordon, Lee: At the point of the fork, we start to go through the queue as if they're all ready

Lee: Until you get to the join at which point the entire chain waits on the previous

Lee: This isn't very long, hopefully it's

Jared: I'm having trouble understanding how 443 woudl change to support what you'd like to do here

Lee: We'd pull it down to the set of Concepts Kirk wants

Lee: We'd add the explicit task construction functions

Lee: Task factory functions with well=defined sets of customization points for this type of task

Jared: Its the refactor of multiple execution functions into a single function + task factories?

Lee: Yes

Mathias: What if the shape is a million?

Lee: Bulk is very much like an openmp parallel for

Lee: I've just implemented a for loop here. a new thread version could issue n threads where n is the number of cores and split the loop up

Lee: The cuda one would just run the cuda task

Mathias: You still need to customize the bulk behavior for each executor, right?

Lee: Sure

Mathias: What's the advantage of the extra indirection versus just a method?

Lee: Scalability later. it's much easier to introduce new types of tasks later

Jared: People who arent paying much attention will only see .execute()

Chris K: This really is just another spelling of properties. it's the same -- we've chosen to make bulk submission a member function, but that's just how we've defined the property

Lee: It's more common in existing libraries

Chris K: Right, but the properties are our way of exposing this type of thing

Jared: We've explored task factories earlier, chrisk was worried about polymorphic executors

Chris K: Right. if we're creating some task with a special interface, and then you take that object and pass to .execute(), how does the poly wrapper preserve that inerface in the common case such that it gets to the underlying executor with those interfaces still avaiable?

Lee: You'd have to erase the token type and task type. i dont think this is any different than type erasing each method of the poly wrapper

Chris K: Right, that's something we're trying to address by moving things into properties

Lee: Yeah. if you want to represent the customization point as properties, you could do that. the only way to get the type safety back is to expose a lot of types through its interface

Chris K: Right now, the wrapper takes a set of property types

Lee: Yeah, but if you're going to type erase the fucntions passed to builk_execute, either you're going to have to pass type erased functions in there and unpack or expose all the potential types you're going to pass in

Chris K: That's the exact problem of having separate task factories

Lee: I'm not clear if you do type erase those

Mathias: Does your objection go away if tasks are objects?

Chris K: I'll have to go back and look at the email jared sent

Mathias: If they're objects with methods, we can erase those

Lee: The tasks are objects with methods

Jared: *shares task factory idea*

Gordon: I think oen of the concerns i have. if we had a single .execute(), would the tasks you create self-contained? or would executors still have to have knowledge?

Lee: In the design i shared, the tasks are not self-contained. they hook into the executor they are run on using customization points

David: In the polymorphic case, this is 2x type erasure because you have two separate functions that do this

Lee: It may be 2x erasure, if you can limit the set of types available to the code, the point in the code where the type of the executor is checked is in the type of the task

Lee: It's right there in the middle of the code with the executor type. but somehow you're going to have to get to the fundamental set of types

Lee: At that point as soon as you choose the right type, all of the code is visible, but you've tied the two things you need for efficient compilation together at the heart of it

Lee: There might be other ways to hook thorugh the poly wrapper, but i dont see how

Chris K: Right now, if this code had like a task.submit() instead of ex.execute(task) at the end, then it would work

Chris K: We already have the mechanism for erasing properties

Mathias: I think there's at least three or four erased things here if i count right

Mathias: What are we gaining over having a method that can be virtual if you want it to be?

Lee: A method on what? if you construct a task against an executor, then type erase it, and then call submit so that you're later submitting it

Lee: You've lost your ability to defer the choise of executor

Lee: If you have a virtual interface to the taskk you've lost the ability to type erase the executor

Mathias: What do you mean by unpack?

Lee: If you need type info on the executor, it's gonna have to be erased on the way in and then selected from a list of types inside

Mathias: That's why a method makes this much simpler

Lee: You can't template the virtual method in any meaningful way

Lee: You can't pass one or other to the other's virtual method

Mathias: I think you can do it if the virtual one is over void

Lee: But then how do you get the untype rased task back later?

Mathias: If you already type erased at both ends, i dont think you can get back

David: Some of this has to do with free functions, right? instead of semicolons

David: Some of this has to do with exposing the sequence of things to the executor all at once

Lee: No, one statment doesnt matter

Lee: You could erase the whole sequence of statements, but that doesnt hep you if the executor was erased at the start

Chris K: Going back to lee's code. what you've wrote could be expressed with properties right now

Chris K: You could use properties to select what youre calling customization points, and it works out of the box with our existing poly wrapper

Lee: I'm unconvinced the polymorphic wrapper actually works. but yes, other than that the properties do work for this

Lee: This is like saying we could have c++ objects implemented with maps as properties, but it's a nonstd way of doing it

Chris K: It really is just a different spelling. with your bulk example, at some point you have to call a function with a particluar signature

Chris K: At some point, someone who wants to customize for their gpu executor, has to write that code

Chris K: What i'm saying is that that signature could be represented as a property in our model

Lee: I acknowledge that

Chris K: So why not do that? anything you'd like to customize with an executor you could do so by writing a property

Chris K: At some point, you've got to write specific code on a specific executor.

David: Chri's point is that properties are more general than a number of ways than free function customization points

David: Properties are a grouping of ways to establish customizations

Kirk: So that justifies making it a completely separate paper that can be sued in general?

David: Thats interesting, but the urgency of the executors effort supercedes that in this case. i do want that paper to be written, and would like this mechanism more broadly used

Kirk: I agree this should be a separate paper, esp if theres a possibility that this mechanism could affect the standardization of executors as a whole

Lee: What if the big LEWG objection to executors was properties?

David: I dont htink that will happen

Jared: I dont recall any LEWG objections to properties

Gordon: I think as well that properties have been shown to be an effective mechanism. there have been a lot of follow on papers that propose additional propertiesk

Chris K: I think a good way forward is to take what you've shown lee and describe them in terms of properties

Chris K: Technically, moving bulk into a property feels like a trick

Lee: It makes more difference when you start building futures on top. from futures POV, bulk is no different from non-bulk

Chris K: If we choose to put these more exotic functions inside of properties, we haven't really reduced the number

Jared: I think everyone agrees that factoring bulk into a property does not actually reduce complexity

Gordon: The approach of having task factories would solve what LEWG was looking for to reduce the number of functions

Gordon: I'd have to see it, but if the tasks were self contained, then i could see the poly executor still working

Gordon: Bc then you could rely on a std interface. as long as they could be separated, but i'm not sure that's possible

Kirk: I belive that would be exatly as capapble as the existin bulk_execute through the poly wrapper, but it would be incredibly instructive to see bulk_execute for something other than cpu

Lee: I cant picture what the code would look like

Lee: Because of the type erasure of the functions, i dont understand how to write bulk_execute on the poly wrapper that successfully runs a cuda task on a cuda executor underneath

Jared: P0443 as it stands now or after some delta?

Chris K: This is just a problem in general.

Lee: Yeah, it applies to both

Chris K: I thought early on that the poly wrapper was intended for cpu-based uses and we weren't trying to make it work for other platforms

Gordon: I'm not sure i entirely understand the problem with P0443 poly wrapper. we have the different execution functions. each of those knows

Gordon: Why do you have to erase the callable of the task?

Lee: You can't template the interface of the underlying executor

Chris K: As it stands, i think that's correct the LUT approach may work in either case

Chris K: It is a more general problem than our poly wrapper

Lee: It is. i think the LUT approach becomes easier if you wrap the whole task and do the lookup inside it than trying to erase the execution functions. or maybe not

Chris K: If we said that the single function was the rawest primitive only, and task factories themselves provide extended submission functions, then i htink we could make that work?

Jared: Who would like to prototype this refactorization?

Chris K: XXX i'll have a go at hacking something up for bulk, i suppose

Lee: What do yoj mean by extended submission methods?

Kirk: If we're only going to have a single function, and do everything else through properties equivalent to existing poitns in the language

Kirk: It doesnt make sense why we'd try to force the property function with the single function

Chris K: I dont understand the last bit

Kirk: Properties should be extracted frmo P0443

David: The argument is that execution is more important -- that's why it deserves searching out of this more general mechanism

Lee: what kirk's saying: Wouldn't it be an easier road to define the fundamentals to get this done in terms of customization points?

David: Properties are the customization point

Kirk: Once we have a requires paper that is accepted, it is always possible to add that support

Jared: Pragmatically, i dont think jettisoning properties will increase consensus

Chris K: No, not at all

David: I agree

Chris K: If properties never went any further than executors, that's not a problem

Gordon: Yeah, i kinda feel like the properties mechanism is the fundamental way that we've supported features through executors. losing that would lose sight of that original purpose

Gordon: Particuarly for our case, removing properties would preclude a lot of our use cases

Mathias: Are we describing what is currently on the screen?

Chris K: Almost. the difference would be the last line. it wouldn't go through the single execute method at that point

Mathias: So at that point you're submitting your jobs through a property

Mathias: Why isnt execute a property?

Chris K: It could be. they could all be properties. i dont know what i think of that either

Lee: One difference between that and what i proposed is that my task definition which accepted values at the end was sitting above all of this

Lee: The trigger onto the executor was happening when the incoming dependencies of the task was satisfied

Lee: Constructing the whole task and passing it to execute and then trying to unpack that again, there's still a difference in the encoding

Lee: I can still imagine doing exactly what i proposed through properties as well

Jared: Anything more to say on this topic?


#### Execution Context EXECUTORS PR #402

*Wording review of [EXECUTORS PR #402](https://github.com/executors/executors/pull/402)*

Jared: Remind us what we want to do with this PR

Carter: To clarify what an EA is, its relation with the callalbe, its relationship with an executor, and then responsibilities for what entity manages the callables

Carter: Qotes the def of EA in [thread.req.lockable.general]

Carter: For the purposes of this library, an EA is the thing which invokes the callable to get it to run

Carter: Within an execution context -- such as a calling thread or thread pool

Carter: An executor is a thing which submits a callable to a context to be invoked by an EA

Jared: Does everyone agree a thread is an execution context?

David: I'm concerned with that in the context of subexecutors. id think you'd often want executors to have inheritance strucutre in terms of subexecutor relationships that might have a common context

David: It makes sense for the context to be the same in all of them, but the place in the hierarchy

Carter: How is that incompatible?

David: Youd want a thread_pool_executor and a thread_executor to both agree on a thread_pool context

David: I dont think we want hierarchies of executors to directly match hierarchies of contexts

Carter: Nothing in there says that. an inline_executor's context is tht thread in which the executor is running

David: I agree with that

Chris K: Would it help to make the such as a note or example? to remove it from normative text?

Carter: Such as is non-normative

Chris K: Maybe move the examples into the note

David: I agree

Gordon: I think incuding this in the normative wording goes down the road of defining what an execution context is

Mathias, Jared: I agree, i would not identify an EA with a thread

Gordon: From that paragraph from the std, i looked into it. i think it's a defect

Jared: Could we avoid copy pasting the definition from the std which we think may have a bug in it

Carter: I put that in here to point out that defect :-)

Gordon: I was thinking of filing a defect report for this as a separate issue

Carter: I'd be happy to do that. let me know why you file it, we can cite it

Gordon: XXX I'll ping you when i do

Carter: Clarifying that an executor submits callables with properties to a context to be run at some point by an EA

Carter: This third paragraph gets a handle on the lifetime of an EA

Carter: The observable lifetime of an EA begins immediately before the callable is invoked and ends immediately after it completes

Carter: So anything affiliated with that agent does not outlive the callable

Mathias: Does it extend to the callable's destructor?

Jared: I dont think so

Carter: Immediately after the invocation of the calalbe completes?

David: What's the distinction between agent and callable?

Carter: When you have a callable which is going to be called many times, but each time it has a different EA calling it

Chris K: Is the word "immediately" too tight?

Carter: Some imaginable scenarios. if you have a callable running in bulk by many EAs, in the dtor of the callable, there is no EA to query its TLS

Carter: It's not there

Chris K: There's a difference in saying we don't guarantee, but we say nothing about saying whether it's there or not

Carter: It's the association with the callable that is broken after the invocation of the callble

David: In that case, agent LS is the same as a stack frame

Mathias: I thought one of the points of ALS was to break lifetime requiresments. you'r required to construct and destroy them, for each thread youre not able to reuse them

Carter: Why is it undesirable for ALS to be created & destroyed with each EA?

Mathias: If i have a thread pool, i may want to create one object for each EA

Jared: Then you want TLS, not ALS

Chris K: Can i suggest that maybe "observable" is too strong?

Chris K: If you try and do something in the dtor, then you're past the end of the lifetime of the EA

Mathias: But the behavior of dtors and ctors is defined

ChrisK: We're talking about the liftime of the EA which encapsulates the invocation

Mathias: Maybe i'm missing what an EA is

Chris K: It's a scope for the properties which apply to the invocation like prioerity, blockingk etc. it defines the scope of the application of properties that were established upon submission

Carter: Yes, & for like bulk it establishes an id

Gordon: The EA also represetnts its place within a parallel execution as well. once you launch a parallel algo you have multiple EAs

Carter: It's possible that the only thing you can see of a bulk-launched agent is its id

Gordon: one thing that comes to mind: If we're discussing whether this lifetime includes the dtor of the callable. does the EA include the destruction of the object as well?

Chris K: I dont want to require the dtor to be within the lifetime. i wonder whether or not to require the dtor to be observably outside the ifteime

Gordon: Makes sense

Jared: Agree

David: I dont understand what we mean by lifetime -- it's deifned by ctor/dtor

Gordon: It defines the lifetime of the callable object when it will be invoked

Carter: heres some of the implictions: You cant find out the identity of an EA outside of the callable invocation

Carter: You cannot access ALS outside the lifetime of the callable invocation

Jared: David, can you suggest an improvement which would avoid ambiguous use of "lifetime"?

David: XXX i can try

Carter: The ctor for the callable (its spelled out later) the ctor of the callable is invoked at the time of submission

Carter: It is run by the submitter's agent. wherever execute is run, that's where the ctor is run

Jared: I'd suggest the dtor runs on an unspecified agent. there's nothing useful you can do in the dtor without the agent's ID, and that is unavailable

Carter: Yep

### Action Items

- (Chris K) Prototype a mechanism for exposing task factories via properties with particular attention paid to their interoperability with polymorphic executors
- (Gordon) File a defect report against the standard's definition of execution agent
- (Carter) Update [EXECUTORS PR #402](https://github.com/executors/executors/pull/402).

