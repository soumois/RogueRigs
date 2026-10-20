---
id: task-245-01-card-hover-reaction
description: "In WBP_UpgradeChoice, replace the instant glow change by a hover animation: tilt following the pointer x (about 4 degrees), scale 1.06, eased return, sheen sweep on blue and above using the existin..."
parent: task-245-card-hover-reaction
kind: task
status: done
---
# 245-01 bp

- goal: In WBP_UpgradeChoice, replace the instant glow change by a hover animation: tilt following the pointer x (about 4 degrees), scale 1.06, eased return, sheen sweep on blue and above using the existing frame textures; update the doc node.
- agent: bp
- depends_on: task-244-02-card-rarity-suspense
- inputs: wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md)
- expected_output: updated widget and doc node
- done_when: compiles with warnings as errors, saved, hover graph read back; doc node updated
- editor_state: open
- token_ceiling: 60000 (provisional)
- model: open
- tokens: open
- runs: 2
- milestone: M2
- result: WBP_UpgradeChoice: hover scale 1.06, tilt pointer x times 4 degrees, eased return 0.15 s, glow eased 0.6 to 1.0, sheen band 0.4 s on blue/purple/yellow stat cards; driven by the gated widget tick (HoverActive); commit a8b2d2fe; compiled warnings as errors, saved; not run or seen (capture in 245-02); first run lost the save to a second editor lock (2 runs). Unverified: pointer x via GetMousePositionOnPlatform.

## Relations
- [[task-245-card-hover-reaction]] — parent global task.
