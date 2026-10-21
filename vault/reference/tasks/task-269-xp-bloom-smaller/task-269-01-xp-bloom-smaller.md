---
id: task-269-01-xp-bloom-smaller
description: "The XP pickups no longer bloom into big glowing blobs: small bright cyan dots with a small halo."
parent: task-269-xp-bloom-smaller
kind: task
status: todo
---
# XP bloom reduced

- goal: The XP pickups no longer bloom into big glowing blobs: small bright cyan dots with a small halo.
- agent: bp
- depends_on: task-268-01-dark-ambience-tight-light
- inputs: lights-dark-ambient (vault/reference/tech/blueprints/world/lights-dark-ambient.md), lights-no-shadows (vault/reference/tech/blueprints/world/lights-no-shadows.md), ppv-xp-glow (vault/reference/tech/blueprints/world/ppv-xp-glow.md), bp-miner (vault/reference/tech/blueprints/player/bp-miner.md), bp-pickup-xp (vault/reference/tech/blueprints/fx/bp-pickup-xp.md), color-code (vault/reference/gdd/art-audio/color-code.md), visual-style (vault/reference/gdd/art-audio/visual-style.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), perf-risks (vault/reference/tech/performance/perf-risks.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), editor-workflow (vault/reference/tech/conventions/editor-workflow.md)
- expected_output: After 268 (fixed exposure): the XP nut and XP cube glow (M_XPNut_hole GlowAmount 25 and the XP sparkle and trail systems NS_PickupSparkle, NS_XPTrail intensity and size, whatever drives the large halo; read the chain from bp-pickup-xp) reduced so an XP pickup reads as a small bright cyan dot (about 12 to 18 px at 1080p) with a small halo; bloom intensity 1.3 on PPV_XPGlow left to the owner unless the cause is the bloom; before and after values recorded; provisional, owner tunes by eye.
- done_when: Before and after values read back; materials and systems compile and are saved; docs updated; lint clean.
- editor_state: open (editor + MCP)
- token_ceiling: 120k
- model: open
- tokens: open
- runs: open
- milestone: M1
- result: open

## Relations
- [[task-269-xp-bloom-smaller]] — parent global task.
