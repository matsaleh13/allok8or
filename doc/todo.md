# allok8or TODO List

## Features

- Synchronization policies:
  - Thread-unsafe.
  - Thread-safe (locking/critsec).
  - Lock-free (maybe).
- Add actual tracking and logging to `diagnostic::AllocationTracker`:
  - Map-like structure to group by data type.
- Allocator Types:
  - Block allocator (i.e. fixed-size-block pool allocator).
  - Linear allocator.
  - Stack allocator (maybe, but only if I can think of a realistic use case).
  - Free List allocator (i.e. variable size block pool allocator). NOTE: ROI on this may not be worth it. If it's easy enough to use multiple block allocators, we may not need it.
  - NOTE: for all the above, special care needed to provide a way to maximize locality and/or provide a way to do so via the API.

## Tech Debt

- More test cases for logging-test.