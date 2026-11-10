---
id: task-499-06-style-review
description: "Style-guide: Critic verdict on the capture frames against the art style guide, colour code and logo rules."
parent: task-499-robot-logo-decals
kind: task
status: done
---
# Yellow Kessler-Voss emblem decal on Heavy, Kamikaze, Chaser v2: style-guide

- goal: Critic verdict on the capture frames against the art style guide, colour code and logo rules.
- agent: style-guide
- depends_on: task-499-05-capture
- inputs: art-style-guide (vault/reference/gdd/art-audio/art-style-guide.md), color-code (vault/reference/gdd/art-audio/color-code.md), kessler-voss-logo (vault/reference/gdd/art-audio/kessler-voss-logo.md)
- expected_output: see done_when
- done_when: Verdict per robot with rule ids; a correction it asks for is applied once and recaptured, or reported to the owner.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus (executor run) + style-guide agent
- tokens: not measured
- usage: not measured
- runs: 1
- milestone: M2
- started: open
- waiting_for: none
- result: Style-guide verdicts on 4 sheets: 2 pass, 2 fail (the 2 fail sheets are the before/after at the game camera and the close-ups; a third fail is the slam ring colour, an older issue). Holds: emblem company yellow, never orange; heavy stripes and kamikaze core stay readable; silhouettes unchanged. Fails reported to the owner, not applied (they re-open owner values or are out of scope): (1) kessler-voss-logo Legibility, A5: at the default game camera no emblem is legible as the logo; heavy and kamikaze show a small yellow mark, chaser v2 and shield chaser v2 show nothing; critic asks 20-22 cm (about half the top width) on the chasers, against the Q3 default of one third (14 cm). (2) Heavy emblem breaks into yellow flecks on its dark recess even close up; critic asks a sharper decal; also visible on the torso piece. (3) Out of scope, older: the heavy slam ring is pale yellow/cream, readability-rules say orange. Earlier task-499-01 review: 2 pass, 0 fail (watch point: outer shards near the rock edge). The script takes --size and --centre-x/-y to apply a new size in one rerun. Cost not recorded (known tooling issue).

## Relations
- [[task-499-robot-logo-decals]] — parent global task.
