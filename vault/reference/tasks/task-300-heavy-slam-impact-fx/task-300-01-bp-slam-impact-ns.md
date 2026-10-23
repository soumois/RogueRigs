---
id: task-300-01-bp-slam-impact-ns
description: "Create NS_HeavySlamImpact, a dust ring and rock chips scaled by User.Radius, with its doc node (bp)."
parent: task-300-heavy-slam-impact-fx
kind: task
status: done
---
# NS_HeavySlamImpact

- goal: New one-shot Niagara system NS_HeavySlamImpact: a flat dust ring expanding to User.Radius (default 361) in about 0.3 s then fading, plus 10 to 20 rock chips thrown up near the centre (M_DigChip look of NS_DigImpact); neutral rock colours, no gameplay colour; doc node bp-heavy-slam-impact under blueprints-fx-index.
- agent: bp
- depends_on: none
- inputs: agent-bp (vault/reference/agents/agent-bp.md), bp-mcp-lessons (vault/reference/agents/bp-mcp-lessons.md), blueprints-fx-index (vault/reference/tech/blueprints/fx/blueprints-fx-index.md), bp-dig-impact (vault/reference/tech/blueprints/fx/bp-dig-impact.md), color-code (vault/reference/gdd/art-audio/color-code.md), enemy-heavy (vault/reference/gdd/enemies/enemy-heavy.md); files: none
- expected_output: /Game/RogueRigs/FX/NS_HeavySlamImpact, doc node bp-heavy-slam-impact
- done_when: compiles without error; User.Radius and values read back; doc node and index relation added; lint clean
- editor_state: open (owner editor, MCP route, do not close it)
- token_ceiling: 30000
- model: sonnet
- tokens: input 60, cache write 82220, cache read 1244388, output 22826
- runs: 1
- milestone: M2
- result: NS_HeavySlamImpact (commit 019ed400): Dust ring 48 puffs, burst once, From Point velocity User.Radius*6.67 with drag 6 (edge ~0.93 R at 0.3 s), grey-brown (0.42,0.35,0.28) a0.5; Chips 14, M_DigChip, cone up 40 deg, gravity -980. User.Radius float default 361. Compile 0 errors. Look unverified.

## Relations
- [[task-300-heavy-slam-impact-fx]] — parent global task.
