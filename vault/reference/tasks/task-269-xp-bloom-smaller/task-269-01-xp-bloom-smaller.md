---
id: task-269-01-xp-bloom-smaller
description: "The XP pickups no longer bloom into big glowing blobs: small bright cyan dots with a small halo."
parent: task-269-xp-bloom-smaller
kind: task
status: done
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
- model: sonnet
- tokens: input 26, cache write 41791, cache read 438712, output 522
- runs: 1
- milestone: M1
- result: XP blob came mainly from the camera-facing glow disc (Radius 50 cm = 100 cm across for a 30 cm nut), plus the emissive nut at GlowAmount 25. Changed: M_XPNut_hole GlowAmount 25 -> 8, MI_XPGlowDisc_Nut Radius 50 -> 22, MI_XPGlowDisc_Chip Radius 68 -> 32 and Intensity 0.5 -> 0.25, M_XPCube_core_bright 12 -> 6 and _dark 6 -> 3; sparkle and trail unchanged (only on collection and while magnetised); bloom intensity and threshold untouched; instance parameter values only, referencers not listed, no recompile. Not seen on screen; next levers: mesh scale, disc Falloff.

## Relations
- [[task-269-xp-bloom-smaller]] — parent global task.
