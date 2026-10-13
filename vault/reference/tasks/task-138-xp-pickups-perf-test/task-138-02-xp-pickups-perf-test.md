---
id: task-138-02-xp-pickups-perf-test
description: "Run the XP pickup cost measurements and record the results in the vault."
parent: task-138-xp-pickups-perf-test
kind: task
status: done
---
# XP pickups: measure and record

- goal: Run the XP pickup cost measurements and record the results in the vault.
- agent: cpp
- depends_on: task-138-01-xp-pickups-perf-test
- inputs: sys-stress (vault/reference/tech/systems/tools/sys-stress.md), sys-stress-run-modes (vault/reference/tech/systems/tools/sys-stress/sys-stress-run-modes.md), perf-stress-test-m1 (vault/reference/tech/performance/perf-stress-test-m1.md), perf-stress-final-m1 (vault/reference/tech/performance/perf-stress-final-m1.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), sys-pickups-xp (vault/reference/tech/systems/world/sys-pickups/sys-pickups-xp.md), build-toolchain (vault/reference/tech/pipelines/build-toolchain.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: a results table in a new doc node in tech/performance (id perf-xp-pickups-glow, kind doc, parent performance-index): for 0, 50, 100, 200 and 400 pickups in total (one chip per 16 pickups), bloom on and bloom off (r.BloomQuality 0): frame, game, draw and GPU ms (average and p99) from the usual launcher (-game, 1920x1080 windowed, stat unit, settle and measure as in perf-stress-test-m1), the thresholds 16.6 ms average and 25 ms p99, the verdict, and the pulse pass: PulseAmount set to 0 on M_XPNut_hole, M_XPChip_core_bright and M_XPChip_core_dark for one run (200 pickups, bloom on), then the three instances restored with git checkout so no asset change is committed; the node listed in performance-index; the wishlist line about the cost check marked done
- done_when: all runs done on the same machine state (no other Unreal process, owner apps as in orchestrator-lessons), the table filled, a verdict per threshold, the three material instances unchanged in git status at the end, lint clean, committed; the results reported to the owner in a short summary
- editor_state: closed
- token_ceiling: open
- model: opus
- tokens: input 98, cache write 231562, cache read 4210637, output 14769
- runs: 1
- milestone: M2
- result: Measured 0 to 400 pickups, bloom on and off, pulse off pass: frame 8.3 to 9.0 ms average, p99 at most 11.3 ms, no measurable cost; the 10 fps seen in 138-01 was a frame-counter misread; assets restored with git.

## Relations
- [[task-138-xp-pickups-perf-test]] — parent global task.
