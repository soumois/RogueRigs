---
id: task-275-03-monday-exposure-measure
description: "One -game run with the Monday lights and the Monday PPV_XPGlow (auto exposure, no mask) measures the adapted EV100 to freeze; PPV restored after."
parent: task-275-monday-light-revert
kind: task
status: todo
---
# Monday auto exposure measured

- goal: the EV100 the auto exposure settles on with the Monday lights, mask off, as the value to put in AutoExposureMin = Max (pre-bias clamp space; bias stays 1).
- agent: cpp
- depends_on: task-275-02-robot-materials-monday
- inputs: ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), agent-cpp (vault/reference/agents/agent-cpp.md); file Content/__ExternalActors__/RogueRigs/Maps/L_ArenaM1/4/WO/G581XYZTDXH1D08OBCBK5D.uasset (PPV_XPGlow; its 75610b21 version has bloom 1.3 / -1, no exposure override, no blendable).
- expected_output: one -game run at the arena start (miner idle at spawn, settled after several seconds) with the Monday PPV package swapped in temporarily; adapted EV100 reported with method and spread; one frame of that run saved under Saved/LookTune/monday_275/; PPV package restored to HEAD (git status clean on it).
- done_when: a single EV100 value with how it was read (engine readout, not an estimate) and its spread over the settled window; the PPV package back to HEAD; no committed code change unless a debug readout was indispensable (then committed by path with its reason); no other run, test or capture.
- editor_state: closed
- token_ceiling: 40k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-275-monday-light-revert]] — parent global task.
