---
id: task-233-01-dig-impact-particle
description: "A particle effect where the drone laser hits the wall: white-gold sparks, chips of the block colour and a little dust while it cuts."
parent: task-233-dig-impact-particle
kind: task
status: in-progress
---
# Dig impact particle

- goal: A particle effect where the drone laser hits the wall: white-gold sparks, chips of the block colour and a little dust while it cuts.
- agent: bp
- depends_on: none
- inputs: drone (vault/reference/gdd/player/drone.md), color-code (vault/reference/gdd/art-audio/color-code.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md), sys-damage-numbers-niagara (vault/reference/tech/systems/combat/sys-damage-numbers-niagara.md), bp-drone (vault/reference/tech/blueprints/player/bp-drone.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md)
- expected_output: Niagara system NS_DigImpact (sparks white-gold, chips with a user colour parameter, dust), its doc node in tech/blueprints/fx/.
- done_when: system compiles and is saved; doc node written; committed.
- editor_state: open
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-233-dig-impact-particle]] — parent global task.
