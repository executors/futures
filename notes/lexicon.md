ISO C++ Executors and Futures
=============================

Lexicon
-------

This document seeks to provide linguistic clarity to the ISO C++ Executors and
Futures standardization project.

| P0443 and P1054 Terms | P1055 Terms | Other Terms | Meaning |
|-----------------------|-------------|-------------|---------|
| Executor | Sender | | An object that creates execution agents to invoke function-like objects. |
| Promise, continuation, function object | Receiver | Task, work | Function-like objects that are invoked by executors. |
| Future | Sender | | An asynchronous value: content (a value or an error) that may not be ready yet. |
| `make_ready_future(x)` | `just(x)` | | An immediately available asynchronous value. |
| `exec.execute(f)` | `exec.submit(f)` | | Use `exec` to invoke `f`. |


