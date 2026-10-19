---
id: task-234-08-duskite-rest-pulse
description: "Duskite crystals: darker mauve at rest, the pulse peak equal to the current look; no capture, the owner checks."
parent: task-234-ore-deposit-look
kind: task
status: in-progress
---
# Duskite rest colour and pulse

- goal: owner 2026-10-19 ("Il faudrait un mauve plus foncé au repos et la pulsation comme l'état de repos maintenant"): the Duskite crystals rest on a darker mauve and pulse up to the look they have now (the current resting brightness becomes the pulse peak). No capture: the owner checks in the editor himself ("Pas besoin de screen je vérifierais moi même").
- agent: bp
- depends_on: task-234-05-deposit-look-capture
- inputs: vault/reference/tech/blueprints/fx/bp-pickup-ore.md, vault/reference/gdd/art-audio/color-code.md, vault/reference/agents/bp-mcp-lessons.md
- expected_output: MI_OreChunk_Bright (and MI_OreChunk_Dark if used) values changed from the M_XPNutGlow formula (Color, GlowColor, GlowAmount, PulseAmount, PulseFrequency): rest = darker mauve, peak = current emissive; parent untouched if possible; doc updated with before and after values.
- done_when: values read back; saved; doc updated; committed.
- editor_state: open
- token_ceiling: 20000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-234-ore-deposit-look]] — parent global task.
