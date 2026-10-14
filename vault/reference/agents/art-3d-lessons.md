---
id: art-3d-lessons
description: "Owner preferences on visuals, agent limits and delegation tips from the art and 3D session of 2026-10-11. Open before art or 3D tasks."
parent: agents-index
kind: note
---
# Art and 3D lessons (2026-10-11)

Complements [[orchestrator-lessons]]. Process pitfalls of the scripts: [[orchestrator-script-quirks]].

## Owner preferences
- Show images: every generated image or render goes to the owner with `SendUserFile`; an agent or the Orchestrator reading a file shows it to nobody. Send a close-up when a detail is discussed.
- Judge by looking: when the owner says a result differs from the reference image, open the reference and crop it close before replying. Five wear attempts failed because the target was misread (the wear is a thin lighter edge, not patches).
- One change at a time per pass; the owner decides each next step. Never add unrequested work.
- Report an unrelated task once, then stop: the owner does not want task reminders from another topic in an art discussion.
- Meshy is not the default; he wants to see what Claude can do in Blender ([[blender-asset-route]]).
- The owner changes the charter when it blocks the result (text on logos, textures for details): state the consequences, offer levels, default to the lightest, then amend the rule and the pipeline node ([[art-style-guide]] A1, A14).
- Only the retained image or mesh is committed; rejected candidates stay untracked.
- A "ok" accepts every proposed default; a question with two readings gets asked once, with a default.

## Agent limits
- Art and 3D agents cannot see: they check by numbers and file contents, the Orchestrator renders and shows ([[blender-asset-route]]).
- A third-party image from the owner is a visual reference only; the agent draws an own version and must not copy it; check the result does not resemble it too closely (the first burst did).
- The permission classifier refuses actions the owner approved only through a relayed message (new network host, commits touching it). Ask the owner to run the command or add a rule in `.claude/settings.local.json`; never route around it ([[kie-image-workflow]]).
- A resumed agent keeps its first label: the cost of later subtasks is counted under it. Prefer one description per subtask when cost tracking matters.
- A bp agent hit the 40-turn limit after two of three groups: ask for groups, a commit per group and a final report even when partial.

## Brief mistakes to avoid
- Do not specify a position rule from a distant case: "bottom of the block, floorless cells included" sank the barrel 200 uu under the floor (task-108). Read the code constants first.
- Specify the camera, light and scale of every preview so renders stay comparable.

## Kie.ai runs (2026-10-14)
- Never run `Tools/art/kie_image.js` in parallel for the same asset: the run id is a timestamp, parallel runs write the same file names and overwrite each other (task-168-02 lost 22 of 24 images, about 144 credits). Run sequentially, one stat or variant set at a time.
- Show candidates as soon as they exist; the owner may reject every proposal and pick another asset as the base (the stat card frame became the weapon frame without its header plate).

## Relations
- [[agents-index]] — parent index.
- [[orchestrator-lessons]] — the general lessons.
- [[orchestrator-script-quirks]] — task script pitfalls.
- [[agent-art]] — the art agent.
- [[agent-3d]] — the 3D agent.
- [[barrel-asset]] — the asset these lessons come from.
