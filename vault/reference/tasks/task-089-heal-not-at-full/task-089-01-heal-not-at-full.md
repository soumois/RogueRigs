---
id: task-089-01-heal-not-at-full
description: "A heal pickup is not taken when the miner is at full health; it stays on the floor until health is below the maximum."
parent: task-089-heal-not-at-full
kind: task
status: done
---
# Heal pickup not taken at full health

- goal: A heal pickup is not taken when the miner is at full health; it stays on the floor until health is below the maximum.
- agent: cpp
- depends_on: task-088-02-heal-veins-2x2
- inputs: heal-ore (vault/reference/gdd/terrain/heal-ore.md), sys-pickups (vault/reference/tech/systems/sys-pickups.md), sys-player (vault/reference/tech/systems/sys-player.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md)
- expected_output: in the pickup collection path (magnet and collect), a heal pickup is ignored while the miner health equals the maximum: it does not move toward the miner, is not collected and is not consumed; it stays where it fell (no despawn added) and is collected normally once health is below the maximum and the pickup is within the pickup radius. A partial heal takes the pickup and the surplus is lost (15 HP at 95 / 100 gives 5 HP). Other pickups (XP, ore) unchanged. Tests: full health leaves the pickup in place, damaged health collects it, partial heal clamps at max. No other gameplay change. Editor closed.
- done_when: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs updated (sys-pickups); lint clean; committed.
- editor_state: closed
- token_ceiling: 50000
- model: opus
- tokens: input 56, cache write 160160, cache read 1517863, output 22407
- runs: 1
- milestone: M2
- result: Heal pickup refused at full health (ARRPickup::IsRefusedByMiner), stays on the floor, re-checked each frame; partial heal clamps; 211/211 tests; two existing tests start at 50 HP. Commits 5dd5e81, bbc1927.

## Relations
- [[task-089-heal-not-at-full]] — parent global task.
