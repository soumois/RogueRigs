---
id: task-243-03-card-deal-flip
description: "From the validated back design make five versions, grey, green, blue, purple, yellow (tier colours of wbp-upgrade-choice SetCard), 1000 x 1440 each; Kie runs sequential."
parent: task-243-card-deal-flip
kind: task
status: done
---
# 243-03 art

- goal: From the validated back design make five versions, grey, green, blue, purple, yellow (tier colours of wbp-upgrade-choice SetCard), 1000 x 1440 each; Kie runs sequential.
- agent: art
- depends_on: task-243-02-card-deal-flip
- inputs: art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), wbp-upgrade-choice (vault/reference/tech/blueprints/ui/wbp-upgrade-choice.md)
- expected_output: five PNG under Art/Source and vault/assets/ once the owner says so, prompts in a vault node
- done_when: five images sent to the owner, same design, only the tier colour changes
- editor_state: any
- token_ceiling: 40000 (provisional)
- model: open
- tokens: open
- runs: 1
- milestone: M2
- result: Five rarity backs (grey = validated base; green, blue, purple, yellow by one correction run each, 24 credits), final PNG 1000 x 1440 transparent in Art/Source/card-back/final/. Prompts in art-prompts-card-back. Cost counted under agent a554d694e71a91c8d (1 run).

## Relations
- [[task-243-card-deal-flip]] — parent global task.
