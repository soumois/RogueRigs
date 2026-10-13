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
- delivery order: creation = `SpawnActorDeferred`, `OnInit`, `FinishSpawning` (BeginPlay), `OnPoolAcquired`, `OnReady`. Reuse = owner, collision, transform (scale = requested scale × the root template scale recorded at creation, as a spawn multiplies them: task-095-01, BP pickups have a 0.3 PickupMesh root and came back 3.3× bigger), visibility, default ticks restored, then `OnInit`, `OnPoolAcquired`, `OnReady`. Release = `OnPoolReleased`, hidden, collision off, actor and component ticks off, timers of the actor cleared, owner cleared.
- spread: each tick serves reuse first (free), then creates, earliest deadline first, max over k of ceil(requests due within k ticks / k): one batch of N with MaxFrames M creates ceil(N / M) per tick. Ticks after the actor tick groups, so a request from an actor tick with MaxFrames 1 is served the same frame. A request whose `Requester` dies is dropped without callback.
- caller contract (tasks 091, 092): implement `IRRPoolable`: `OnPoolAcquired` resets per-life state (BeginPlay runs only once: health, death flags, scale, registration in subsystems such as `URRPickupSubsystem`), `OnPoolReleased` stops activity and unregisters. Replace `Destroy` with `Release`. Keep the handle and cancel it when the requester stops (miner death, level end). Systems that enumerate or count live actors skip `IsPooledIdle(Actor)` (spawner `AliveEnemies` only holds delivered robots; actor iterators must filter).
- data: none (MaxFrames comes from each caller's tunables).
- depends_on: none.
- implements: [[perf-trace-real-game]] recommendations 2 and 3 (pool robots, pool XP cubes).
- callers: robots (task-091-01, [[sys-spawner]] `SpawnMaxFrames` 20, [[sys-enemies-base]] reset hook); pickups (task-092-01, [[sys-pickups]]: XP cubes, ore and heal drops through `URRPickupSubsystem`, `SpawnMaxFrames` 3, requester = the pickup subsystem, released on collection; task-110-01: merged big XP blocks are another pooled class acquired the same way, with the same root template scale rule; game 250: 433–587 created and about 2500 reused per 60 s, longest wait 3 frames; S7 burst 1200 cubes created 400 per frame). Measured in [[stress-grid-cells]] (game 250): about 325 robots created and 809 to 863 reused per 60 s window, longest wait 15 to 16 frames.
- gotchas: game thread only. Reuse turns collision on before the transform is restored: a caller that shrinks its actor must restore its scale in `OnPoolReleased` (robots do; otherwise Chaos ensures NewMP.Mass > 1e-8, a 3 s hitch). `Release` refuses actors not created by the pool, already idle, or being destroyed (warning). Members destroyed by anyone leave the pool (`OnDestroyed`). DontSpawnIfColliding is not honoured on reuse. World end: `Deinitialize` drops pending requests without callbacks and leaves idle actors to the world.

## Relations
- [[systems-index]] — parent index.
- [[sys-spawner]] — robot spawning to migrate (task-091).
- [[sys-enemies-base]] — robot death and DropXP to migrate.
- [[sys-pickups]] — pooled pickups (task-092-01) and their reset hooks.
- [[sys-automation-tests]] — RogueRigs.ActorPool.* tests.
- [[perf-trace-real-game]] — spawn costs that motivate the pool.
