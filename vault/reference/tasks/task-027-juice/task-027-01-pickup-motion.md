---
id: task-027-01-pickup-motion
description: "XP and ore chunks eject in arcs, rest, then stream into the miner; collect event; kamikaze 0.5 s flare before exploding; tests."
parent: task-027-juice
kind: task
status: in-progress
---
# Pickup motion and kamikaze flare

- goal: pickups move like DRGS and the kamikaze telegraphs.
- agent: cpp
- depends_on: task-026-04-tests-integration
- inputs: [[drgs-analysis]], [[sys-pickups]], [[sys-progression]], [[sys-enemies]], [[sys-explosions]], [[cpp-standard]], [[sys-automation-tests]], [[drgs-analysis]]
- expected_output: pickups: on robot death spawn N XP cubes splitting its XP (chaser 3, kamikaze 4, shooter 6, heavy 12 cubes; values add up to the robot XP), each launched in a random arc 80–220 uu outward over 0.35 s with a small bounce, then resting; ore and heal blocks pop 3–5 chunks the same way; magnet: inside pickup radius a pickup accelerates toward the miner (start 400 uu/s, accel 4000 uu/s², max 2500) so nearby cubes form a stream; collect event per pickup (position, type) for a sparkle; pooled or cheap movement (no physics); kamikaze: flare phase 0.5 s before the proximity explosion (death explosion stays instant), event OnFlareStart and flare progress getter; docs sys-pickups, sys-enemies-kamikaze. Damage number follow-ups from stress-damage-numbers-recheck: CriticalColor linear (1, 0.55, 0.08) so crits read gold on screen (confirmed by the owner after handover-2026-10-06); avoid crowd overlap by stacking a new number vertically when another open number is within its footprint. Tests `RogueRigs.Pickups.Motion*` and kamikaze flare timing.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 110000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: built (7837267): XP bursts 3/4/6/12 cubes, arc + bounce + rest + magnet via URRPickupSubsystem, OnPickupCollectedAt, kamikaze flare events/getters, crit gold, number stacking; 175/175 tests; S7 400-chaser burst avg 9.19 p99 11.34 ms. Ore and heal pop one chunk per block (GDD), not 3-5.

## Relations
- [[task-027-juice]] — parent global task.
