---
id: sys-actor-pool
description: "Generic actor pool world subsystem: reuse of released actors, creation spread over MaxFrames, IRRPoolable reset hooks, cancel, IsPooledIdle filter, counters."
parent: systems-index
kind: doc
---
# Actor pool

- responsibility: hand out actors of any class, reusing released ones at once and spreading the creation of missing ones over at most MaxFrames ticks; park released actors; counters for tests and stress logs (task-090-01). Callers migrate in task-091 (robots) and task-092 (pickups).
- main_classes: `Source/RogueRigs/Pool/`: `URRActorPoolSubsystem` (`RRActorPoolSubsystem.h`, tickable world subsystem, game and PIE worlds); `IRRPoolable` (`RRPoolable.h`, BlueprintNativeEvent `OnPoolAcquired`, `OnPoolReleased`); plain structs `FRRPoolAcquireParams` (Owner, Instigator, CollisionHandling, MaxFrames, Requester, `OnInit`, `OnReady`), `FRRPoolRequestHandle`, `FRRActorPoolStats`.
- entry_points: `Acquire(Class, Transform, Params)` → handle (idle actor delivered before returning, else queued); `AcquireImmediate(Class, Transform, Params)` (synchronous, outside the budget); `Prewarm(Class, Count, MaxFrames)` (default none); `CancelRequest(Handle)`; `IsRequestPending`; `Release(Actor)`; `IsIdle`, `IsPoolMember`, static `IsPooledIdle(Actor)`; `GetStats(Class = null)`, `ResetStats`; `ClearPool(bDestroyIdle)`.
- delivery order: creation = `SpawnActorDeferred`, `OnInit`, `FinishSpawning` (BeginPlay), `OnPoolAcquired`, `OnReady`. Reuse = owner, collision, transform, visibility, default ticks restored, then `OnInit`, `OnPoolAcquired`, `OnReady`. Release = `OnPoolReleased`, hidden, collision off, actor and component ticks off, timers of the actor cleared, owner cleared.
- spread: each tick serves reuse first (free), then creates, earliest deadline first, max over k of ceil(requests due within k ticks / k): one batch of N with MaxFrames M creates ceil(N / M) per tick. Ticks after the actor tick groups, so a request from an actor tick with MaxFrames 1 is served the same frame. A request whose `Requester` dies is dropped without callback.
- caller contract (tasks 091, 092): implement `IRRPoolable`: `OnPoolAcquired` resets per-life state (BeginPlay runs only once: health, death flags, scale, registration in subsystems such as `URRPickupSubsystem`), `OnPoolReleased` stops activity and unregisters. Replace `Destroy` with `Release`. Keep the handle and cancel it when the requester stops (miner death, level end). Systems that enumerate or count live actors skip `IsPooledIdle(Actor)` (spawner `AliveEnemies` only holds delivered robots; actor iterators must filter).
- data: none (MaxFrames comes from each caller's tunables).
- depends_on: none.
- implements: [[perf-trace-real-game]] recommendations 2 and 3 (pool robots, pool XP cubes).
- gotchas: game thread only. `Release` refuses actors not created by the pool, already idle, or being destroyed (warning). Members destroyed by anyone leave the pool (`OnDestroyed`). DontSpawnIfColliding is not honoured on reuse. World end: `Deinitialize` drops pending requests without callbacks and leaves idle actors to the world.

## Relations
- [[systems-index]] — parent index.
- [[sys-spawner]] — robot spawning to migrate (task-091).
- [[sys-enemies-base]] — robot death and DropXP to migrate.
- [[sys-pickups]] — pickup spawn queue to migrate (task-092).
- [[sys-automation-tests]] — RogueRigs.ActorPool.* tests.
- [[perf-trace-real-game]] — spawn costs that motivate the pool.
