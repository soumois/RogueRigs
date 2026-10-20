---
description: Run the style loop on generated game text. Score every item (voice, lore, reference, format), rewrite only the failing items, re-score, stop at 8 or after 2 refinements.
argument-hint: <run-id> <stem>:<content>:<log> [...]
---
Run the style loop (node `style-loop` in the vault). Arguments: $ARGUMENTS

You drive the loop. The script decides pass or fail; the agents only score and rewrite.

1. Read `style-guide-text` and `style-loop` in the vault by descent (`_navigation.md`). Read nothing else yourself.
2. `node Tools/style/style_loop.js init Tools/style/runs/<run-id> --run-id <run-id> --source <stem>:<content>:<log> ...`
3. Repeat:
   a. `node Tools/style/style_loop.js check <runDir>` (format findings).
   b. Launch the `style-guide` agent in scored mode on `<runDir>/current/*.json`; it writes `<runDir>/round<N>.score.json`.
   c. `node Tools/style/style_loop.js gate <runDir> --scores <runDir>/round<N>.score.json`. On exit 1, send the message back to the evaluator once; if it repeats, stop and report.
   d. `done` or `escalate`: go to step 4. `refine`: for each stem in `round<N>.gate.json`, launch the `text` agent with ONLY that file's failing items (text, rule, reason, suggestion), the stem, and the output paths `<runDir>/refined/round<N>/<stem>.json` and its log. It keeps every field but the text field, runs the validator until exit 0, and logs retrieval for the rules it applied. Then `apply <runDir> --stem <stem> --refined <file> --log <log>`.
4. `node Tools/style/style_loop.js report <runDir>`; give the owner the final status, the table of corrections, and any escalation.

Never edit a content file or a rule yourself. Never lower the threshold to finish.
