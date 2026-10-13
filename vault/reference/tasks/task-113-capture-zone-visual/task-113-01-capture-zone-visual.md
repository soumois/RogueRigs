---
id: task-113-01-capture-zone-visual
description: "Console and chest zones use the ground telegraph: sharp zone circle, growing progress circle, rules unchanged."
parent: task-113-capture-zone-visual
kind: task
status: in-progress
---
# Capture zone visual (C++ side)

- goal: The console and chest hack zones show a sharp zone circle and a growing progress circle; the old diffuse circle is gone; the rules are unchanged.
- agent: cpp
- depends_on: task-112-01-ground-telegraph
- inputs: objective-console (vault/reference/gdd/run/objective-console.md), objective-bonus-chest (vault/reference/gdd/run/objective-bonus-chest.md), ground-telegraph (vault/reference/gdd/art-audio/ground-telegraph.md), sys-objectives (vault/reference/tech/systems/sys-objectives.md), bp-objective-console (vault/reference/tech/blueprints/bp-objective-console.md), bp-bonus-chest (vault/reference/tech/blueprints/bp-bonus-chest.md), sys-automation-tests (vault/reference/tech/systems/sys-automation-tests.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), cpp-bp-split (vault/reference/tech/conventions/cpp-bp-split.md)
- expected_output: owner 2026-10-12 (rendering only; the rule "marked rocks must be destroyed before hacking" is unchanged): use the ground telegraph (task-112-01) on ARRConsole (and ARRBonusChest, which derives from it): a component created in the constructor, radius = the zone radius in uu (console 9 cells x 67 = 603, chest 6 cells = 402; read the actual ZoneRadiusCells), mode ZoneBlocked while OnMarkedRocksChanged says Remaining > 0 and ZoneActive when Remaining = 0 (also for the chest, which has no marked rocks: active at once), progress = the hack progress (follows OnHackProgress, including the slow drain while the miner is out of the zone, so the growing circle shrinks), hidden when the hack is complete or the object is destroyed; the old diffuse growing circle is a Blueprint effect (find it in bp-objective-console / bp-bonus-chest and sys-objectives: say exactly which component or event draws it); do NOT edit Blueprints here: if it can be turned off from C++ (a flag) do it, otherwise list precisely what task-113-02 (bp) must remove. Tests: mode switches when the last marked rock is destroyed, progress follows the hack and drains, hidden at completion, chest active at once. Docs sys-objectives, sys-ground-telegraph, sys-automation-tests. Editor closed.
- done_when: build exits 0 with the editor closed; all RogueRigs.* tests pass headless; docs updated; lint clean; committed.
- editor_state: closed
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-113-capture-zone-visual]] — parent global task.
