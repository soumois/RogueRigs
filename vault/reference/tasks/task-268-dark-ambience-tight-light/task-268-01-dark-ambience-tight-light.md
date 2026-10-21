---
id: task-268-01-dark-ambience-tight-light
description: "The ambience is dark around a tight warm pool of light on the player, like the reference: shorter light radius, much dimmer ambient, fixed exposure."
parent: task-268-dark-ambience-tight-light
kind: task
status: todo
---
# Dark ambience and tight player light

- goal: The ambience is dark around a tight warm pool of light on the player, like the reference: shorter light radius, much dimmer ambient, fixed exposure.
- agent: bp
- depends_on: none
- inputs: lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), color-code (vault/reference/gdd/art-audio/color-code.md), visual-style (vault/reference/gdd/art-audio/visual-style.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), perf-risks (vault/reference/tech/performance/perf-risks.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: PlayerLight on BP_Miner: AttenuationRadius 1400 -> about 650 and an intensity that keeps a strong warm core; ambient much dimmer: DirectionalLight_0 intensity 1.0 -> about 0.12 and SkyLight_0 intensity 0.5 -> about 0.08, still violet; the exposure no longer adapts to the dark (auto exposure raised it back: the owner screenshot shows a bright pink violet arena): fixed exposure on PPV_XPGlow (auto exposure min = max brightness, or manual with a bias; state the method and the value, tunable); every previous value recorded for revert in lights-dark-ambient; values provisional, owner tunes by eye; the reference pool is about 0.18 of the screen width (about 400 uu at the camera distance 1400 and fov 75).
- done_when: Values read back before and after (PlayerLight, DirectionalLight_0, SkyLight_0, PPV exposure); compile and save OK; docs updated; lint clean.
- editor_state: open (editor + MCP)
- token_ceiling: 120k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-268-dark-ambience-tight-light]] — parent global task.
