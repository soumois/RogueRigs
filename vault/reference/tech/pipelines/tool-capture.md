---
id: tool-capture
description: "Tools/capture scripts: capture.js (-game run, report), sheet.js (labelled comparison sheet, crops), commands.js (rr.* list), ffmpeg finder. Open to run a visual check."
parent: pipelines-index
kind: doc
---
# Capture tooling (Tools/capture)

Node 18+, no dependencies. Source: task-359 ([[task-359-capture-tooling]]).

- capture.js: `node Tools/capture/capture.js --name <Name> --exec "<ExecCmds ending with rr.Debug.Delayed <s> Quit>" [--prefix a,b] [--expect N] [--keep <regex>] [--ignore <regex>] [--map <path>] [--res WxH] [--force-res] [--timeout s] [--max-lines N] [--cols N] [--no-sheet]`. One -game run; copies the PNGs to `Saved/Captures/<Name>/`, builds `contact_sheet.png`, prints about 15 lines; full log in `run.log`. Exit 0 done, 1 timeout, crash or missing shots, 2 bad args. Known limit: its contact sheet shows only the first tile when screenshots have different resolutions; use sheet.js for multi-resolution comparisons.
- MSYS repair: Git Bash rewrites an argument starting with `/` into `C:/Program Files/Git/...`, so `--map /Game/...` would arrive broken. capture.js (`repairMsysMap`, `repairMsysExec`, task-402) turns an explicit `--map` (not none, not default) matching `<drive>:/[...]Git/[usr/]Game/<rest>` (case-insensitive) back into `/Game/<rest>` and prints `map: repaired MSYS path -> /Game/<rest>`; in `--exec`, such prefixes become `/Game/` and it prints `exec: repaired MSYS path(s) -> <exec>`. A raw `UnrealEditor.exe` launch from Bash needs `MSYS_NO_PATHCONV=1` in front.
- sheet.js: `node Tools/capture/sheet.js --out <png> [--cols N] [--height H] [--crop x,y,w,h] [--zoom Z] [--no-crops] <label>=<image> | <image> ...`. Labelled grid, images scaled to a common height, mixed resolutions accepted. Crop values all <= 1 are fractions of the frame, else pixels; the crop applies to every input. With `--crop`, each crop is also written as `<out name>-<label>.png` (`--no-crops` skips these). Default cols = image count, max 3; default height 540 (or the first crop height) times zoom. Prints 2 to 3 lines. Exit 0 done, 1 ffmpeg missing or failed, 2 bad args.
- commands.js: `node Tools/capture/commands.js [filter]`. Filter = case-insensitive regex on name, help text or system folder. Scans `Source/RogueRigs` .cpp and .h for `rr.*` console commands and variables. Output grouped by system folder: `[cmd]` or `[var = default]`, help, file:line, then a summary line (90 entries on 2026-11-03: 35 commands, 55 variables). Cheat list of the game: [[sys-cheats]].
- ffmpeg.js: shared by capture.js and sheet.js. `findFfmpeg()` tries PATH, then the first WinGet `ffmpeg.exe` under `%LOCALAPPDATA%/Microsoft/WinGet/Packages`, result cached; `imageSize()` reads the size from the PNG header, else from ffmpeg.
- aim-dependent captures (task-423): the aim follows the mouse or stick, so fix it with `rr.Debug.Aim <YawDegrees>` in the case commands (world yaw 0 = +X, 90 = +Y; `off` clears it; [[sys-aim-indicator]]). The camera command must be in the command list of each case: the camera persists across cases within a run.
- UI screen captures (task-424): open the screen by debug command, then shoot with the UI: `node Tools/capture/capture.js --map /Game/RogueRigs/Maps/L_ArenaM1 --res WxH --force-res --prefix pm --expect 1 --exec "rr.Cheat.Invulnerable 1,rr.Debug.Delayed 4 rr.Debug.PauseMenu open,rr.Debug.Delayed 6 rr.Debug.Shots 0 1 1 pm 1,rr.Debug.Delayed 9 Quit"`; the command is `rr.Debug.PauseMenu open|resume|exit` ([[sys-pause-menu]]).
- ui-screens.js (task-549): `node Tools/capture/ui-screens.js --name <Name> [--screens mainmenu,pause,options,stats,shop,death,runwon] [--res 1920x1080] [--compare <BeforeName>] [--list]`. One capture.js run per group (menu; pause+options; stats+shop; death; runwon = level 4 stats), cursor parked in a corner; frames as `Saved/Captures/<Name>/<screen>.png`. `--compare` writes `compare-<screen>.png` (before | after) and prints the PSNR per screen (inf = identical; shop cards and the level behind run-won are random, so judge those by eye). 5 launches, about 4 min at 1920 x 1080. Inventory of the shared UI pieces: [[ui-shared-pieces]].
- rules for agents: capture count and image limits are in [[agent-cpp]] (`capture_rules`).

## Relations
- [[pipelines-index]] — parent index.
- [[agent-cpp]] — runs captures through `capture_script`.
- [[sys-cheats]] — the in-game cheats commands.js lists beside the other rr.* entries.
- [[sys-pause-menu]] — owner of rr.Debug.PauseMenu.
- [[sys-aim-indicator]] — owner of `rr.Debug.Aim`.
- [[ui-shared-pieces]] — ui-screens.js and the shared UI pieces it checks.
- [[task-359-capture-tooling]] — task that added sheet.js, commands.js and ffmpeg.js.
