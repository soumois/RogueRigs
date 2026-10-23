---
id: task-301-01-console-c2-files
description: "Save the console C2 sheet, Meshy files and prompt (art): console c2 uplink mast mesh, step 01."
parent: task-301-console-c2-mesh
kind: task
status: done
---
# Save the console C2 sheet, Meshy files and prompt

- goal: Commit the owner C2 files and record them: sheet copied to vault/assets/console-c2-turnaround.webp; Meshy FBX and maps and the three owner reference images committed in Art/Source/console-meshy/; new node art-prompts-console under art-prompts with the prompt verbatim, owner words, decisions 2, 3, 5, 6 and the file list; validated image line added to art-prompts.
- agent: art
- depends_on: none
- inputs: agent-art (vault/reference/agents/agent-art.md), art-3d-lessons (vault/reference/agents/art-3d-lessons.md), art-prompts (vault/reference/gdd/art-audio/art-prompts.md), art-prompts-robots-meshy (vault/reference/gdd/art-audio/art-prompts-robots-meshy.md), art-prompts-drone (vault/reference/gdd/art-audio/art-prompts-drone.md); files: scratchpad decisions file (prompt and owner words, copy the verbatim prompt into the node): C:/Users/soumo/AppData/Local/Temp/claude/C--Users-soumo-Documents-Unreal-Projects-RogueRigs/026d99ce-bb6f-41c6-a632-ca35213400f0/scratchpad/console-c2-owner-decisions.md; Art/Source/console-meshy/owner/console-c2-turnaround.webp, Art/Source/console-meshy/owner/ref1-mobile-part.png, Art/Source/console-meshy/owner/ref2-meshy-top-feet-lights-current.png, Art/Source/console-meshy/owner/ref3-meshy-top-feet-lights-wanted.webp, Art/Source/console-meshy/meshy/Meshy_AI_Satellite_Receiver_St_1023222045_texture.fbx with _texture.png, _normal.png, _metallic.png, _roughness.png, _metallic_roughness.png
- expected_output: vault/assets/console-c2-turnaround.webp; committed Art/Source/console-meshy/ files; node art-prompts-console; art-prompts relation and validated image line
- done_when: files tracked in git; art-prompts-console holds the prompt verbatim and the file list; relations on both ends; lint clean; committed
- editor_state: any
- token_ceiling: 20000
- model: sonnet
- tokens: input 22, cache write 27002, cache read 223028, output 5573
- runs: 1
- milestone: M2
- result: Owner sheet, 3 reference images, Meshy FBX and 5 maps committed in Art/Source/console-meshy/ (plain git, like drone-meshy); sheet copied to vault/assets/console-c2-turnaround.webp; node art-prompts-console with verbatim prompt, owner words, decisions 2,3,5,6 and file list; commit d00cd406.

## Relations
- [[task-301-console-c2-mesh]] — parent global task.
