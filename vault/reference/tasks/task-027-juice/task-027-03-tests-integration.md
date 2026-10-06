---
id: task-027-03-tests-integration
description: "Integration tests with the final Blueprints: kill drops cubes that stream in, kamikaze flares before exploding, heal ring event."
parent: task-027-juice
kind: task
status: in-progress
---
# Juice integration tests

- goal: the juice features are covered.
- agent: cpp
- depends_on: task-027-02-bp-juice
- inputs: [[sys-automation-tests]], task-027-02 result.
- expected_output: `RogueRigs.Integration.*` tests with real Blueprints; full run. Added after task-027-01: damage-number stacking also avoids numbers already displayed and still rising (read the display lifetime from NS_DamageNumbers; screenshot Juice_06 showed two white numbers overlapping); a -game shot with an armed kamikaze flare (force a kamikaze next to the miner). Added after task-027-02: -game shots (open and describe) of the XP trail and collect sparkle, the heal ring, the kamikaze CoreLight flare (peak 60000 is a guess: say if it blows out), the console HackRing and the extraction chevrons (check the arrows point toward the shuttle, not sideways), and damage numbers over yellow robots and grey floor (say whether a dark outline is needed). Report Blueprint fixes precisely for a later bp pass; do not edit .uasset files.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless (new ones included); docs updated; lint clean.
- editor_state: closed
- token_ceiling: 70000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-027-juice]] — parent global task.
