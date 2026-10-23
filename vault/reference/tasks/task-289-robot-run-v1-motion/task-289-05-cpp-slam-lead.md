---
id: task-289-05-cpp-slam-lead
description: "Heavy slam-down clip starts early enough for the contact pose at the new RunRate (bp)."
parent: task-289-robot-run-v1-motion
kind: task
status: todo
---
# Heavy slam lead for the new RunRate (BP default)

- goal: The heavy strike clip (clip 2, looped by the shader with RunRate) reaches its contact frame 4 on the slam frame at the new heavy RunRate 0.7 (4 / (30 x 0.7) = 0.190 s): set SlamContactLead 0.20 s (EditDefaultsOnly, C++ default 0.15) on the BP_Heavy class defaults; no C++ change (the owner has the editor open, a build is blocked).
- agent: bp
- depends_on: task-289-04-bp-chaser-v2-run
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), bp-heavy (vault/reference/tech/blueprints/enemies/bp-heavy.md), sys-enemies-heavy (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies-heavy.md); files: Source/RogueRigs/Enemies/RRHeavy.h (read only)
- expected_output: BP_Heavy saved with SlamContactLead 0.20; bp-heavy doc updated (sys-enemies-heavy gets one line: BP_Heavy overrides it)
- done_when: CDO value read back 0.20; compiles without error; docs updated; lint clean
- editor_state: open
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-289-robot-run-v1-motion]] — parent global task.
