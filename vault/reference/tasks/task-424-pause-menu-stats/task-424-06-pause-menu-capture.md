---
id: task-424-06-pause-menu-capture
description: "Capture: -game captures of the pause menu at four resolutions against the validated mockup, Resume and Exit by debug command"
parent: task-424-pause-menu-stats
kind: task
status: todo
---
# Pause menu: miner stats screen with Resume and Exit: capture

- goal: Check the pause menu on screen against the validated mockup and the acceptance.
- agent: capture
- depends_on: task-424-05-pause-menu-widget
- inputs: task-424-pause-menu-stats (vault/reference/tasks/task-424-pause-menu-stats.md) (acceptance, Q7), `vault/reference/gdd/art-audio/art-prompts-pause-menu-mockup.md` (new, written by subtask 01) (validated mockup), agent-capture (vault/reference/agents/agent-capture.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md) (weapon and upgrade cheats to own two weapons and take a global bonus), debug command `rr.Debug.PauseMenu open|resume|exit`, capture flag `Tools/capture/capture.js --force-res` for 1920x1200 and 2560x1080
- expected_output: Frames under `Saved/` at 1920x1080, 1280x720, 1920x1200 and 2560x1080 with the menu open mid-run (two weapons owned, at least one global bonus taken), plus frames after resume and after exit; a report per point seen, not seen or ambiguous with frame paths.
- done_when: Report gives C1 to C8: C1 the menu is drawn over the dimmed paused arena and its layout follows the validated mockup; C2 title PAUSED; C3 the global block shows the six miner stats and five global weapon bonuses with values; C4 one block per owned weapon (two) with damage, fire rate, reload, crit chance, crit damage, magazine and pierce; C5 Resume and Exit buttons with their text inside the plates; C6 no clipping or overlap at 1280x720, 1920x1200 and 2560x1080; C7 after rr.Debug.PauseMenu resume the menu is gone and the game runs; C8 after rr.Debug.PauseMenu exit the main menu (L_MainMenu) is shown. Ambiguous points go to the owner.
- editor_state: closed
- token_ceiling: 0.4 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-424-pause-menu-stats]] — parent global task.
