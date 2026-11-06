---
id: task-457-02-radar-frame
description: "Bp: The radar widget gets its frame with a place for the ore count."
parent: task-457-radar-frame-ore
kind: task
status: done
---
# Radar frame holding the ore count: bp

- goal: The radar widget gets its frame with a place for the ore count.
- agent: bp
- depends_on: task-457-01-art
- inputs: wbp-minimap-locator (vault/reference/tech/blueprints/ui/wbp-minimap-locator.md), sys-hud-minimap (vault/reference/tech/systems/ui/sys-hud-minimap.md), umg-layout (vault/reference/tech/conventions/umg-layout.md), Art/Source/hud-unified/
- expected_output: see done_when
- done_when: Compiled and saved; radar unchanged inside; PIE shows no runtime error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-457
- usage: included in the executor run of task-457
- runs: 1
- milestone: M2
- started: 2026-11-06T00:34:23Z
- waiting_for: none
- result: Commit 436ec2b1. T_HUD_RadarFrame imported to /Game/RogueRigs/UI/HUD (UI group, no mips). WBP_MinimapLocator: new Image RadarFrame (not a variable, HitTestInvisible), brush T_HUD_RadarFrame, canvas slot anchors 0,0, alignment 0,0, offsets 16,16 size 524 x 297 (sprite 1024 x 580 at 0.512), z 6 after the disc so the ring covers the disc edge, below the markers (7, miner 8) so clamped markers stay visible. Ring hole centre at 172,163 diameter ~196: MinimapImage moved 20,20 -> 72,63 (200 x 200 unchanged), the four markers 120,120 -> 172,163 (marker translations are relative, graphs unchanged). Plate inside (for 03): x 312..491, y 78..159. Compiled with warnings as errors, saved. PIE L_ArenaM1: no LogScript error or Accessed None. Not seen on screen (Q26).

## Relations
- [[task-457-radar-frame-ore]] — parent global task.
