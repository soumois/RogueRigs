---
id: task-283-11-kamikaze-core-scale
description: "Set ArmedCoreScale 1.2 on BP_Kamikaze so the armed Meshy core no longer swallows the robot (bp)."
parent: task-283-robot-mesh-integration
kind: task
status: in-progress
---
# Kamikaze armed core scale on the Meshy core

- goal: On BP_Kamikaze set the class default ArmedCoreScale to 1.2 (provisional, owner 2026-10-22; C++ default 1.8 and game rules unchanged), because the 102 cm Meshy core shell grown 1.8 times swallowed the robot (283-10 kamikaze_pulse_04).
- agent: bp
- depends_on: task-283-10-capture-after
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md), bp-kamikaze (vault/reference/tech/blueprints/enemies/bp-kamikaze.md), sys-enemies-kamikaze (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-kamikaze.md)
- expected_output: BP_Kamikaze saved; bp-kamikaze updated (before 1.8 inherited, after 1.2 provisional owner 2026-10-22, revert)
- done_when: ArmedCoreScale read back 1.2 on the BP default object, compiles without error, no C++ test expects the BP value (checked), doc updated; lint clean
- editor_state: open
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-283-robot-mesh-integration]] — parent global task.
