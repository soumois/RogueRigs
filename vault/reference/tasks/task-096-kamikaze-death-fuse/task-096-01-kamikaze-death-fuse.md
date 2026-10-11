---
id: task-096-01-kamikaze-death-fuse
description: "A kamikaze killed by the player goes through the same 0.65 s preparation as a proximity trigger and releases its XP only at the explosion."
parent: task-096-kamikaze-death-fuse
kind: task
status: todo
---
# Kamikaze death fuse

- goal: A kamikaze killed by the player goes through the same 0.65 s preparation as a proximity trigger and releases its XP only at the explosion.
- agent: cpp
- depends_on: task-100-01-ore-rename-duskite
- inputs: enemy-kamikaze (vault/reference/gdd/enemies/enemy-kamikaze.md), sys-enemies-kamikaze (vault/reference/tech/systems/sys-enemies/sys-enemies-kamikaze.md), bp-kamikaze (vault/reference/tech/blueprints/bp-kamikaze.md), sys-explosions (vault/reference/tech/systems/sys-explosions.md), explosions (vault/reference/gdd/terrain/explosions.md), sys-enemies-base (vault/reference/tech/systems/sys-enemies/sys-enemies-base.md), sys-damage (vault/reference/tech/systems/sys-damage.md), sys-actor-pool (vault/reference/tech/systems/sys-actor-pool.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md)
- expected_output: owner decisions 2026-10-10 (already written in enemy-kamikaze): a kamikaze whose health reaches 0 from damage dealt by the player side (weapons, turrets, drone) does not explode at once nor die: it enters the Armed state as if triggered by proximity (movement off, committed, the existing core flare via OnFlareStart / OnFlareStarted for the 0.65 s fuse), then queues its explosion; it cannot be killed again during the fuse (further damage ignored) and it is not counted as alive for the cap only once it has actually died; its XP is released only at the explosion, never at the kill; a death caused by an explosion (chain reaction, barrel, another kamikaze) keeps the instant explosion. Keep everything else: radius, damage, explosion queue (one per 0.08 s), pooled robots (release after the explosion, reset hook clears the fuse state), proximity trigger unchanged. Check the interaction with: the kill credit / state.Dead tag, the spawner alive count, kill-count HUD or objectives if any, the Blueprint events (OnFlareEnd bDetonatedByFuse true). Update the tests that assume an instant death explosion; new tests: kill by weapon starts the fuse and explodes 0.65 s later, XP only at explosion, kill by explosion stays instant, no double explosion, no XP twice. Docs sys-enemies-kamikaze, sys-explosions, sys-automation-tests. Editor closed; no Blueprint change unless the flare event cannot be reused (then report it).
- done_when: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs updated; lint clean; committed.
- editor_state: closed
- token_ceiling: 60000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-096-kamikaze-death-fuse]] — parent global task.
