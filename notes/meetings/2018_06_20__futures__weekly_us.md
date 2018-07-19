ISO C++ Futures
===============

Weekly US Timezone Meeting - Notes - 2018-06-20
-----------------------------------------------

**Kindly forgive and identify any mistakes and omissions in these notes.**

Attendees: **???**

Agenda:
- Debrief from Rapperswil/CSCS workshop discussion of futures/post-Rapperswil Executors telecon.
- Discuss `then`/`then_execute` taking promises.
- Review work on prototype/reference implementation.
- Assign work for P1054 update for post-meeting mailing (adding more examples as requested by SG1).
- Triage issues/milestones.

### Open Questions (Not Addressed at This Meeting)

- What type of futures can `then_execute` take as a parameter?
- How do we specialize for "foreign" future types that we want to handle specially?
- Can we remove `twoway_execute`?
- What type of thing should `then_execute`/`then` take as a parameter?

