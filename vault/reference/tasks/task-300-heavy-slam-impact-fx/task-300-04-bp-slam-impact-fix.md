---
id: task-300-04-bp-slam-impact-fix
description: "Fix NS_HeavySlamImpact after the 300-03 capture: ring too small, raised off the feet, whitish, chips invisible (bp)."
parent: task-300-heavy-slam-impact-fx
kind: task
status: in-progress
---
# Fix NS_HeavySlamImpact from the capture

- goal: The 300-03 capture (Saved/RobotMeshes/slam-impact/slam_sheet.png) shows the dust ring reaching only ~150 uu (0.4 of 361), drawn ~90 px up-screen of the feet (raised or offset), near-white instead of grey-brown, and chips barely visible specks. Find the causes (User.Radius reaching the system, HLSL speed and drag, shape offset, colour or material, chip size) and fix NS_HeavySlamImpact so the ring reaches about User.Radius on the ground at the feet, reads grey-brown, and the chips read as rock chips; update bp-heavy-slam-impact (before, after).
- agent: bp
- depends_on: task-300-03-capture-slam-impact (capture run 1)
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-heavy-slam-impact (vault/reference/tech/blueprints/fx/bp-heavy-slam-impact.md), bp-heavy (vault/reference/tech/blueprints/enemies/bp-heavy.md), bp-dig-impact (vault/reference/tech/blueprints/fx/bp-dig-impact.md); files: Saved/RobotMeshes/slam-impact/slam_sheet.png
- expected_output: NS_HeavySlamImpact fixed (BP_Heavy only if the cause is there), doc nodes updated
- done_when: causes named; values read back; compiles without error; doc updated; lint clean; committed
- editor_state: open
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-300-heavy-slam-impact-fx]] — parent global task.
