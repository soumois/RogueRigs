---
id: task-359-01-capture-tooling-scripts
description: "Cpp: Tools/capture/sheet.js for crops and labelled comparison sheets with the ffmpeg finder shared with capture.js, and commands.js listing rr.* console commands."
parent: task-359-capture-tooling
kind: task
status: todo
---
# Capture tooling: scripts

- goal: Write `Tools/capture/sheet.js` and `Tools/capture/commands.js` as decided in the global `## Questions` Q5 and Q6; move `findFfmpeg` of capture.js to a shared module both use. Node 18+, no dependencies, short output. No game run, no C++ change, no doc node edits.
- agent: cpp
- depends_on: none
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), global task-359-capture-tooling (vault/reference/tasks/task-359-capture-tooling.md), `Tools/capture/capture.js`, sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), task-355-02-shop-card-frame-details-capture (vault/reference/tasks/task-355-shop-card-frame-details/task-355-02-shop-card-frame-details-capture.md) (the crops and comparison sheet it built by hand), existing PNGs in `Saved/ShopFix/task-355/` for the test
- expected_output: the scripts committed by path; change summary with usage lines and the test outputs.
- done_when: sheet.js run on three PNGs of `Saved/ShopFix/task-355/` writes a labelled sheet and fraction crops (outputs in Saved/, not committed); `commands.js shop` lists `rr.Shop.Buy` and `rr.Shop.Reroll` with their file; `commands.js` with no filter lists the rr.Cheat, rr.Debug, rr.Run, rr.Menu commands; capture.js usage unchanged and its contact sheet still builds through the shared finder (checked on existing PNGs, no game run); committed.
- editor_state: any
- token_ceiling: 0.8 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-359-capture-tooling]] — parent global task.
