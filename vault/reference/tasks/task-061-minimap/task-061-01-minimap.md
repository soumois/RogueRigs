---
id: task-061-01-minimap
description: "Round minimap at the top left: seen cells of rock, floor and heal veins, markers for miner, console, chest and shuttle."
parent: task-061-minimap
kind: task
status: in-progress
---
# Minimap

- goal: Round minimap at the top left: seen cells of rock, floor and heal veins, markers for miner, console, chest and shuttle.
- agent: cpp
- depends_on: none
- inputs: readability-rules (vault/reference/gdd/art-audio/readability-rules.md), sys-hud (vault/reference/tech/systems/sys-hud.md), sys-grid (vault/reference/tech/systems/sys-grid.md), sys-objectives (vault/reference/tech/systems/sys-objectives.md), color-code (vault/reference/gdd/art-audio/color-code.md), cpp-standard (vault/reference/tech/conventions/cpp-standard.md)
- expected_output: C++ data for a minimap: a 240 × 240 texture (UTexture2D or render target, one pixel per cell; rock, floor, heal vein, void colours from color-code) updated only on cell changes and as cells become seen (seen = inside the camera view once); unseen cells transparent or dark; a getter giving the miner cell and the marker positions (console, bonus chest, shuttle) in texture space, and the view radius (about 40 cells); cheap per frame; Blueprint-readable API listed exactly for task-061-02; update only existing tests that break; docs sys-hud.
- done_when: build exits 0 with the editor closed; all `RogueRigs.*` tests pass headless; docs updated; lint clean.
- editor_state: closed
- token_ceiling: 70000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-061-minimap]] — parent global task.
