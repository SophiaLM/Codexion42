*This project has been created as part of the 42 curriculum by sophluna.*
# Codexion
---

## Description

**Codexion** is a multithreaded concurrency simulation written in C (POSIX threads), built as part of the 42 curriculum. It places `N` programmer threads (*coders*) around a ring of hardware **dongles**: a coder cannot compile unless it holds **two dongles at the same time** — its own and its neighbor's — which turns the program into a live exercise in resource contention, scheduling and deadlock avoidance.

### Goal

The objective of the project is to implement a correct, deadlock-free concurrent system where:

- Every coder cycles through the states *has taken a dongle → is compiling → has released the dongles → is debugging → is refactoring*.
- Access to each dongle is arbitrated fairly by a per-dongle scheduler (mutex + condition variable + priority queue).
- No thread can deadlock, starve or corrupt shared state, regardless of the configuration.
- The simulation terminates **by itself**, cleanly, once every coder has completed the required number of compilations.

### Overview

Each dongle behaves like a small receptionist: it keeps a waiting line (a binary min-heap of requests), knows whether it is busy (`holder`) and whether it is cooling down (`available_at_ms`). When it becomes free it grants itself to whoever is **at the head of the queue** — not to whoever shouts loudest. Two schedulers are supported:

| Scheduler | Queue key | Behavior |
|---|---|---|
| `fifo` | Arrival ticket (`next_ticket++`) | Strict first-come, first-served |
| `edf` | Deadline (`last_compile_start + time_to_burnout`) | Most exhausted coder is served first |

Dongles are assigned in a ring: coder *i* uses dongle *i* and dongle *(i+1) mod N*. All output is serialized into atomic lines `<elapsed_ms> <coder_id> <message>`.

**Current scope:** parsing, ring assignment, FIFO/EDF scheduling, cooldown, natural termination when all coders reach their compile quota, and clean shutdown from any error path are fully implemented and tested. A dedicated burnout-monitor thread (forced termination of unsolvable configurations using `BURNOUT_TOLERANCE_MS`) is designed and is the next milestone; with an unsolvable configuration the program stalls gracefully instead of crashing.

## Instructions

### Requirements

- A POSIX-compliant system (Linux/macOS) with `cc` and `make`.
- No external libraries: only the C standard library and `pthread`.

### Build

```bash
cd coders
make        # builds the `codexion` binary (-Wall -Wextra -Werror -pthread)
make clean  # removes object files
make fclean # removes objects + binary
make re     # full rebuild
```

### Run

```bash
./codexion <number_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <fifo|edf>
```

| Argument | Meaning | Constraint |
|---|---|---|
| `number_of_coders` | Number of coder threads (and dongles) | > 0 |
| `time_to_burnout` | Max tolerated ms between two compilations | > 0 |
| `time_to_compile` | Duration of a compilation (ms) | > 0 |
| `time_to_debug` | Duration of a debugging session (ms) | > 0 |
| `time_to_refactor` | Duration of a refactoring session (ms) | > 0 |
| `number_of_compiles_required` | Compiles each coder needs before the simulation ends | > 0 |
| `dongle_cooldown` | Cooldown (ms) a dongle enters after being released | ≥ 0 |
| `fifo` / `edf` | Dongle queue scheduler | case-sensitive, lowercase |

Examples:

```bash
./codexion 5 800 200 200 200 5 50 fifo   # viable configuration with cooldown
./codexion 2 400 100 100 100 5 0 fifo    # tight but viable pair, no cooldown
./codexion 4 310 200 100 100 10 0 fifo   # unsolvable: stalls gracefully, never crashes
```

Invalid input (wrong count, non-numeric values, zero/negative values, overflow above `INT_MAX`, unknown scheduler) produces an error message on `stderr` and exits with code `1`.

The simulation ends with exit code `0` as soon as **every** coder reaches `number_of_compiles_required`.

### Tests

Unit and integration shell tests live in `tests/`:

```bash
cd tests
bash test_heap.sh            # min-heap: push/pop/peek + removal from any position
bash test_time_logging.sh    # clock, interruptible sleep, stop flag, logging
bash test_coder_routine.sh   # coder lifecycle + shutdown wakes blocked threads
bash test_log_format.sh      # log line format and monotonic timestamps
bash test_fifo.sh            # viable run + 20x stress executions (deadlock hunt)
```

## Blocking cases handled

This section describes every concurrency hazard the solution explicitly guards against.

### Deadlock prevention — resource ordering (Coffman conditions)

Every coder acquires its two dongles in **ascending id order** (`take_dongles`, `src/dongles/dongle_take.c`), regardless of which one was assigned as "first". This breaks the **circular-wait** condition (4th of Coffman): a cycle would require some coder to hold a high-id dongle while waiting for a lower-id one, which the acquisition order forbids. The other three Coffman conditions still hold by design — mutual exclusion (per-dongle mutex), hold-and-wait (the first dongle is kept while waiting for the second) and non-preemption (a dongle is only released voluntarily) — but with the fourth one broken, no cycle can close.

A second, subtler deadlock case is handled: with `number_of_coders == 1` a coder's two dongles are the same object (`first == second`). The `low != high` check prevents locking the same mutex twice from one thread, i.e. self-deadlock without needing a second thread at all.

### Starvation prevention

- **Broadcast, never signal.** `release_dongle` uses `pthread_cond_broadcast`: waking an arbitrary waiter (`pthread_cond_signal`) could wake someone who is not at the head of the queue; that waiter would re-sleep and the rightful head might never be woken again — a silent permanent block. Broadcast wakes everyone, the head proceeds, the rest re-sleep; the "stampede" cost is bounded by N waiters and negligible.
- **Strict queue discipline.** Requests are served strictly by heap key (arrival ticket under FIFO), so a waiter can never be overtaken indefinitely; every enqueued request eventually reaches the head.
- **EDF urgency.** Under `edf`, keys are burnout deadlines, so the coder closest to burning out is served first and cannot be starved behind fresher requests.
- **Bounded waits everywhere.** Condition waits use `pthread_cond_timedwait` with a 10 ms budget (`COND_POLL_MS`) and sleeps are sliced into 200 µs steps, so no thread ever blocks unboundedly on anything except a genuinely held dongle.

### Cooldown management

On release, a dongle sets `available_at_ms = now_ms() + cooldown` **under its own mutex and before broadcasting** (`src/dongles/dongle_release.c`). `my_turn` refuses to grant the dongle while `now_ms() < available_at_ms`. Consequences:

- A released dongle cannot be instantly re-grabbed, which gives neighbors a fair window and models realistic hardware behavior.
- The cooldown check happens inside the same critical section as `holder`/queue inspection, so there is no window where a dongle appears free during its cooldown.
- With `cooldown = 0` the mechanism degenerates gracefully to immediate availability (regression-tested).

### Accurate exhaustion detection

- Each coder records `last_compile_start` (and increments `compile_count`) **atomically** under its personal `state_mutex` at the exact moment compilation starts (`set_compile_start`). Exhaustion bookkeeping is therefore never torn or stale.
- The EDF key `last_compile_start + time_to_burnout` turns that timestamp into a precise per-request urgency value consumed by the dongle scheduler.
- Termination checks are evaluated **only at safe points** (the loop header): a coder whose quota is complete finishes its current iteration — releasing both dongles and completing debug/refactor — before exiting, so detection never interrupts a critical section or leaves a `holder` dangling.
- Shutdown awareness has a hard upper bound of ~10 ms (`COND_POLL_MS`): any waiter notices a stop within that window, keeping end-of-simulation latency deterministic.
- `BURNOUT_TOLERANCE_MS` defines the tolerance margin reserved for the upcoming forced-burnout monitor.

### Log serialization

All output goes through `log_state`, which takes the global `print_mutex` so that a full line (`timestamp id message`) is emitted atomically — interleaved characters from different threads are impossible. Two additional disciplines apply:

- Logging happens **outside** the dongle mutex ("has taken a dongle" is logged after unlocking `d->mtx`), so the slow console can never extend a critical section nor couple the print lock with a dongle lock.
- The global lock order is fixed: `print_mutex → stop_mutex` (via `sim_stopped` inside `log_state`). No code path takes them in the opposite order, so the logging subsystem itself can never deadlock against the stop flag.
- After the simulation stops, `log_state` prints nothing, guaranteeing a clean, consistent final log.

### Clean abandonment of queues

If the simulation stops while a coder waits in a dongle queue, it must leave **without corrupting the heap**. Its request may sit anywhere (not just at the root), so `leave_or_take` uses `heap_remove(heap, coder_id)` — a search-and-repair removal — instead of `heap_pop`, then unlocks. Additionally, `coder_routine` re-checks `sim_stopped` immediately after `take_dongles`, so a woken-by-shutdown coder never compiles holding dongles it does not actually own.

## Thread synchronization mechanisms

### Primitive inventory

| Primitive | Where | Protects |
|---|---|---|
| `pthread_mutex_t mtx` (one per dongle) | `t_dongle.mtx` | Dongle state: `holder`, `available_at_ms`, request heap, `next_ticket` |
| `pthread_cond_t cond` (one per dongle) | `t_dongle.cond` | Waiters sleeping for their turn on that dongle |
| `pthread_mutex_t state_mutex` (one per coder) | `t_coder.state_mutex` | `last_compile_start`, `compile_count` |
| `pthread_mutex_t stop_mutex` | `t_sim.stop_mutex` | The global `stop` flag |
| `pthread_mutex_t print_mutex` | `t_sim.print_mutex` | stdout — one atomic log line at a time |
| Min-heap request queue (custom) | `t_dongle.queue` | Waiting-line order (FIFO ticket or EDF deadline key) |
| Timedwait polling pattern (custom event layer) | `wait_for_turn`, `smart_sleep` | Interruptible waits: shutdown acts as an event even though nobody broadcasts on shutdown |

### Mutexes: one owner per piece of state

Each piece of mutable state has exactly one designated mutex, and every read or write of that state occurs while holding it:

- `holder`, `available_at_ms`, the heap and the ticket counter are **never** touched outside `d->mtx`. Example — release is one atomic step, so no thread can observe "free but still in cooldown" inconsistently:

```c
pthread_mutex_lock(&d->mtx);
d->holder = 0;
d->available_at_ms = now_ms() + cooldown;
pthread_cond_broadcast(&d->cond);
pthread_mutex_unlock(&d->mtx);
```

- `last_compile_start` / `compile_count` are written by their owner thread (`set_compile_start`) and read by the termination check (`compiles_done`) and by the EDF key computation (`compute_key`) — always under the same `state_mutex`, so writer and readers never race:

```c
/* writer */                          /* reader */
pthread_mutex_lock(&me->state_mutex);       pthread_mutex_lock(&me->state_mutex);
me->last_compile_start = now_ms();          deadline = me->last_compile_start
me->compile_count++;                                   + cfg->time_to_burnout;
pthread_mutex_unlock(&me->state_mutex);     pthread_mutex_unlock(&me->state_mutex);
```

- The `stop` flag flips only inside `stop_mutex` (`sim_stop` / `sim_stopped`); because every loop, wait and log consults it through these functions, stop is observed coherently by all threads.

### Condition variables: wake-up ≠ my turn

The canonical acquire pattern in `take_dongle` shows how races are avoided around turn-taking:

```c
pthread_mutex_lock(&d->mtx);
req.coder_id = me->id;
req.key = compute_key(me, d);
heap_push(&d->queue, req);          /* enter the line — atomically with the check below */
wait_for_turn(me, d);               /* while (!my_turn && !stopped): cond_timedwait */
leave_or_take(me, d);               /* pop + become holder, OR abandon the queue */
```

Three properties make this race-free:

1. **Enqueue-and-check atomicity.** Pushing the request and inspecting `my_turn` happen under the same `d->mtx`; a releaser broadcasting in between cannot slip past us — either we already hold the mutex (we re-check after waking) or we are asleep *with the mutex released*, which is precisely the atomicity gap `pthread_cond_wait` exists to close.
2. **`while`, never `if`.** Waking up only means "re-check": another thread may have taken the turn, or the wakeup may be spurious (explicitly allowed by POSIX). The loop tolerates both.
3. **Grant under lock, log outside.** `my_turn` verifies three facts in one critical section (free ∧ out-of-cooldown ∧ I am the head). Only then does the winner `heap_pop`, set `holder = me->id` and unlock; logging happens afterwards so the console never runs inside the dongle critical section.

### The custom event layer: timedwait polling

When the simulation shuts down, nobody broadcasts on the dongle condition variables (doing so would require locking every dongle mutex from the stopping path, crossing `stop_mutex` with dongle locks — a deliberately rejected design). Instead, waits are made *interruptible by time*:

```c
while (!my_turn(me, d) && !sim_stopped(me->sim))
{
	ts = deadline_ms(COND_POLL_MS);              /* absolute +10 ms */
	pthread_cond_timedwait(&d->cond, &d->mtx, &ts);
}
```

Each timeout expiry acts as a synthetic "tick" event: the waiter re-evaluates the predicate, including `sim_stopped()`. This gives the system a shutdown event with bounded (~10 ms) latency, zero busy-waiting (measured ~2 % CPU with three threads), and no cross-locking. The same idea powers `smart_sleep`, which slices sleeps into 200 µs steps and aborts early when the stop flag rises.

### Global lock ordering

There are exactly two places where two mutexes are held at once, both with globally consistent order and no reverse path anywhere in the codebase:

- `compute_key` (under an already-held dongle mutex): `d->mtx → state_mutex`
- `log_state`: `print_mutex → stop_mutex`

Everything else follows one rule: **never sleep and never take a second mutex while holding a dongle mutex**. This is why no lock-order cycle can form among the project's own primitives.

### Thread-safe communication between coders and the monitoring logic

Termination is decided cooperatively through shared, mutex-guarded state rather than through direct thread-to-thread signaling:

1. Each coder publishes its progress (`compile_count`, `last_compile_start`) under its `state_mutex` — the monitoring checks (`compiles_done`, EDF keys) read the same fields under the same mutex.
2. When every quota is met, each coder simply exits its loop; `main` joins all threads, so the process ends exactly when the last coder finishes.
3. Any error path (e.g. `pthread_create` failure in `cleanup`) calls `sim_stop()`: the flag flips under `stop_mutex`, every running coder observes it at its next check (loop header, log call, sleep step or 10 ms cond tick), waiters abandon their queues via `heap_remove`, and already-created threads are joined before resources are destroyed — no leaked thread, no leaked mutex, no double free (`destroy_dongles` nulls the array defensively).

Because every channel is a mutex-protected field plus a monotonic flag, there is no moment where a coder and the monitoring logic disagree about the simulation state.

## Resources

### Classic references

- **E. W. Dijkstra — *Cooperating Sequential Processes* (1965):** the founding text on mutual exclusion; origin of the dining-philosophers model that the coder/dongle ring mirrors.
- **E. G. Coffman Jr., M. J. Elphick, A. Shoshani — *System Deadlocks* (1971):** the four Coffman conditions and deadlock prevention via resource ordering (§ *Blocking cases handled*).
- **C. L. Liu & J. W. Layland — *Scheduling Algorithms for Multiprogramming in a Hard-Real-Time Environment* (1973):** the original statement of Earliest-Deadline-First scheduling, used for the `edf` mode.
- **A. Silberschatz, P. Galvin, G. Gagne — *Operating System Concepts*:** deadlock characterization, prevention and recovery chapters.
- **A. S. Tanenbaum — *Modern Operating Systems*:** threads, race conditions and deadlock chapters.
- **M. Kerrisk — *The Linux Programming Interface*, ch. 29–31:** definitive practical treatment of pthreads, mutexes and condition variables on Linux.
- **A. Downey — *The Little Book of Semaphores* (free online):** condition-variable patterns, lost-wakeup problems, deadlock exercises.
- **LLNL POSIX Threads Programming tutorial** (Lawrence Livermore National Laboratory): classic hands-on pthread reference.
- **POSIX.1-2017 / man pages:** `pthread_create(3)`, `pthread_mutex_lock(3)`, `pthread_cond_wait(3)`, `pthread_cond_timedwait(3)`, `pthread_cond_broadcast(3)` — semantics relied upon (spurious wakeups, mutex-release-on-wait atomicity).
- **42 common-core *Philosophers* subject:** the closest sibling project — same ring-of-shared-resources structure, useful for comparing strategies.

### Use of AI

AI tools were used as an assistant throughout the project, always with the student authoring the decisions and the final code:

- **Concept tutoring (Parts 5–7):** walking through mutex/condition-variable semantics, the atomicity contract of `pthread_cond_wait`, spurious wakeups, and why `broadcast` beats `signal` in a queued system — applied while writing `src/dongles/`.
- **Design review of synchronization strategy:** reviewing the global lock-order analysis (`d->mtx → state_mutex`, `print_mutex → stop_mutex`), validating the resource-ordering anti-deadlock argument against the Coffman conditions, and weighing `cond_timedwait` polling versus broadcast-on-shutdown (decision documented in `notes_dongles.md`).
- **Debugging support:** interpreting `valgrind` leak summaries (including why leak-checks after signal-death are unreliable), reading `gdb` backtraces of stuck threads, and reasoning about probabilistic stress-test failures in `tests/test_fifo.sh`.
- **Repository audit and status reconstruction:** an AI-assisted full-repo review produced `ponte_al_dia.md` (per-module responsibilities, verified test matrix, risk list) used to plan the remaining work.
- **Planning and documentation:** drafting the day plans (`plan_dia_06.md`, `plan_dia_07.md`), study notes (`notes_dongles.md`), termination design notes (`hacer_que_pare.md`) and this README.
- **Test design:** proposing scenario matrices for the shell tests — invalid-input batteries, log-format invariants, repeated stress runs and hang detection with timeouts.

Architecture choices (ring assignment, resource ordering, FIFO/EDF keys, cooldown semantics), all implementation and every final commit are the student's own work; AI served as reviewer, explainer and documentation aid.
