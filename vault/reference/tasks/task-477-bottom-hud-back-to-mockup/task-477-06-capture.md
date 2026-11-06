---
id: task-477-06-capture
description: "Capture: Final independent check of the bottom HUD against the mockup at the resolutions of Q3."
parent: task-477-bottom-hud-back-to-mockup
kind: task
status: done
---
# Bottom HUD redone from the art proposal, up to 6 correction rounds: capture

- goal: Final independent check of the bottom HUD against the mockup at the resolutions of Q3.
- agent: capture
- depends_on: task-477-05-match-loop
- inputs: agent-capture (vault/reference/agents/agent-capture.md), Art/Source/hud-unified/20261105-152627-hud-unified-1.png
- expected_output: see done_when
- done_when: C1..C8 each get a verdict on frames at the Q3 resolutions; side-by-side sheets mockup / result sent to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-477
- usage: included in the executor run of task-477
- runs: 1
- milestone: M2
- started: 2026-11-06T19:56:07Z
- waiting_for: none
- result: 1 launch at 1920x1080 only (Q3), editor closed: Saved/Captures/t477_06_final (f_00 at 6 s, f_01 after rr.Debug.DamageMiner 30 and rr.Cheat.GrantXP 3); no script error. C1 seen (one block centred x 592..1331, top y 842, strip 43 and rail 62 tall: within 1 px of the mockup at 1080p); C2 seen (weathered teal-green metal, rust streaks and chips, the mockup's own pixels); C3 seen (small corner and edge rivets, stepped shoulder plates, 8-rivet Lv plate as the mockup); C4 seen (bright green strip on the slot row, '100 / 100' white bold dark outline centred); C5 seen (four equal 163 x 108 cells, dark insides, rifle and cream reload bar in cell 1, teal padlocks centred in 2-4); C6 seen (dark track, riveted end caps, Lv 1 plate at the left, four white ticks at 20/40/60/80 %); C7 seen (nothing overlaps or leaves the screen); C8 seen ('70 / 100' with the damage chunk inside the bar, cyan fill and pale ghost inside the rail). Ambiguous: none. Sheets for the owner: Saved/Captures/t477_06_final/sheet-bottom.png (mockup / before / result / after damage+XP), sheet-block.png (zoom), sheet-full.png.

## Relations
- [[task-477-bottom-hud-back-to-mockup]] — parent global task.
