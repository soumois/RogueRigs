---
id: task-415-04-capture-aim-laser
description: "Capture: capture the aim laser in a -game run and check C1 to C7"
parent: task-415-aim-laser
kind: task
status: done
---
# Aim laser replaces the white aim chevrons: capture

- goal: Check the aim laser in a -game run at 1920x1080 against the checklist in done_when and the reference frames; report each point seen, not seen or ambiguous with the frame or crop path.
- agent: capture
- depends_on: task-415-03-delete-aim-chevrons
- inputs: task-415-aim-laser (vault/reference/tasks/task-415-aim-laser.md), aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md), `Art/Source/aim-indicator/owner-test-2026-10-27-b.webp` (look before: chevrons), `Art/Source/aim-indicator/laser-reference-2026-11-02/sheet.png` (reference), `Art/Source/aim-indicator/laser-reference-2026-11-02/f01.png` (reference)
- expected_output: Frames and crops under Saved/, a report per point C1 to C7.
- done_when: C1 no chevron in front of the miner. C2 a thin straight white-gold line starts at the rifle muzzle tip and runs along the aim direction. C3 the line is continuous above the floor, never cut by it or sunk into it. C4 with a robot on the aim line within 1200 uu the line ends at that robot; with a rock block and no nearer robot it ends at that block; with open floor and no robot it runs about 1200 uu and fades out at its tip. C5 a bright core with a soft glow of the same colour, not red. C6 two frames with different aim directions show the line following the aim. C7 no reticle at the line end. Each point reported seen, not seen or ambiguous with its frame path.
- editor_state: Unreal editor closed (-game run)
- token_ceiling: 0.4 M units
- model: sonnet
- tokens: input 26, cache write 47314, cache read 341463, output 4205
- usage: turns 12, context start 13k peak 47k, resumes 2 (message x2), 4 min, units 0.11 M
- runs: 0
- milestone: M3
- started: 2026-11-02T17:58:50Z
- waiting_for: none
- result: Validated by the owner (2026-11-02, "Ok, je valide ce que j'ai vu"): capture run interrupted, C1 to C7 not checked by the capture agent, closed on his word

## Relations
- [[task-415-aim-laser]] — parent global task.
