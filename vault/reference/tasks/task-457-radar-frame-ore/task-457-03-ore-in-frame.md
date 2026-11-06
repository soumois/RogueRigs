---
id: task-457-03-ore-in-frame
description: "Bp: The ore count shows in the part of the radar frame that extends to take it in; the floating counter is gone."
parent: task-457-radar-frame-ore
kind: task
status: done
---
# Radar frame holding the ore count: bp

- goal: The ore count shows in the part of the radar frame that extends to take it in; the floating counter is gone.
- agent: bp
- depends_on: task-457-02-radar-frame
- inputs: wbp-ore-counter (vault/reference/tech/blueprints/ui/wbp-ore-counter.md), wbp-hud (vault/reference/tech/blueprints/ui/wbp-hud.md), sys-objectives-ore-counter (vault/reference/tech/systems/world/sys-objectives/sys-objectives-ore-counter.md), umg-layout (vault/reference/tech/conventions/umg-layout.md)
- expected_output: see done_when
- done_when: Compiled and saved; count updates on pickup in PIE; no runtime error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-457
- usage: included in the executor run of task-457
- runs: 1
- milestone: M2
- started: 2026-11-06T00:37:31Z
- waiting_for: none
- result: Commit 193b562b. WBP_OreCounter: OreIcon slot 236,30 -> 322,119 (56 x 56, anchors 0,0, alignment 0,0.5), OreText slot 300,58 -> 386,119 (auto size, alignment 0,0.5, Roboto Bold 32 unchanged); both centred on the plate inside of the radar frame (x 312..491, y 78..159); graph, binding and ShowOre unchanged; OreCounter paints above MinimapLocator (same z 6, later child). Nothing in C++ or other widgets pointed at the old counter position. Compiled with warnings as errors, saved. PIE L_ArenaM1: rr.Cheat.GrantOre 10 logged '+10 ore, carried 10, secured 0' through AddCarriedOre/OnOreChanged (the pickup path); no LogScript error or Accessed None. The OreText value itself was not read back (PIE widget path not reachable through MCP). Offline layout composite (not a game frame) Saved/Scratch/hud-radar/layout-check-457.png: disc fills the ring hole, icon and count inside the plate. Not seen on screen (Q26).

## Relations
- [[task-457-radar-frame-ore]] — parent global task.
