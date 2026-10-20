# Assignment 6 — Style Guide Agent

**ELVTR · Multi-Agent AI for Game Development** · Soumois Pierre-Emmanuel

⬅️ [Assignment 5](../A5-goal-oriented-coding-agent/) · [All assignments](../)

> **Status: ready to submit.** Due October 26, 2026, 16:59 Pacific (23:59 GMT). Every number below comes from the run logs in [`Tools/style/runs/20261020-a6-run3/`](../../Tools/style/runs/20261020-a6-run3/).

## The assignment

Build the evaluator loop for *Rogue Rigs*: a **Style Guide Agent** that encodes the game's own aesthetic rules and enforces them on content generated for the game. The evaluator returns a **score and a reason**, not a pass/fail; the refiner uses the reason to fix the content on its own. The loop must show the agent **correcting** violations, not only detecting them.

## Rubric and where each criterion is answered

| Criterion | Points | Section |
|---|---|---|
| Game specificity | 3.0 | [The style guide](#the-style-guide) |
| Enforcement accuracy | 3.0 | [The evaluator and the refiner](#the-evaluator-and-the-refiner) |
| Before/after demonstration | 2.0 | [Three before/after examples](#three-beforeafter-examples) |
| Style guide depth | 1.5 | [The style guide](#the-style-guide) |
| Pipeline connection | 0.5 | [Where it runs](#where-it-runs) |

## The style guide

The content under review is the **player-facing text** of the game: Rook and Vesna's barks, Kessler-Voss alert-bar lines and upgrade flavor lines. The guide is not new: it groups the numbered rules the project already had (game design document and vault: `dialogue-tone` T1-T9, `lore` L1/L2/L4, `company-voice`, `crew-characters`, `fictional-films`) into four constraint types and adds scoring. Source of the grouping: vault node `style-guide-text`.

| Constraint type | What it judges | Rules | Where the rule comes from |
|---|---|---|---|
| **Voice** | The line sounds like its speaker: Rook cocky and jargon-heavy, Vesna calm and dry, Kessler-Voss never joking, jokes aimed at the company or robots, never at the player. | T2, T4, T5, T6 | Owner decisions of 2026-10-09 / 10-13 in the GDD; `crew-characters`, `company-voice` |
| **Lore** | The line fits the world: every enemy is a robot, Vesna only speaks through the drone, the Rigs know the claim is not theirs yet, company text never orders the player to stop. | L1, L2, L4, T9 | `lore`; T9 was added after a playtest misread "Cease all extraction" (A4 history) |
| **Reference** | Film quotes come only from the in-universe catalogue (`fictional-films`) with original lines; no real film line word for word (a clear twist into the Rigs' world is wanted, owner rule of 2026-10-20); past jobs only from `crew-characters`; no vault example reused. | T3, T8 | `dialogue-tone`, `fictional-films` |
| **Format** | Barks 3-8 words, other lines 3-12, English ASCII, no digit in flavor, any number must come from a retrieved vault excerpt, a quoted film must be in the catalogue. Checked by code, never by a model. | T1, T7, F1-F4 | A4 validator; F1-F4 added in A6 |

Four constraint types, each tied to the capstone's own design documents; a generic "be fun" rule exists nowhere in the guide.

## The evaluator and the refiner

- **Generator:** the text agent (sonnet). **Evaluator:** the `style-guide` agent in scored mode (opus, a different model from the generator). **Refiner:** the text agent again, on failing items only. **Gate:** the script `Tools/style/style_loop.js`, plain code.
- **Score:** every item gets an integer 0-10 for voice, lore and reference from the evaluator, and a format score (10 or 0) from the validator. 9-10 on-brand, 8 acceptable, 5-7 partly off, 0-4 clear violation. Every item carries a **reason** (passes included); a failing item also carries the rule ids and a **suggestion**.
- **Threshold:** an item passes when its lowest dimension is at least 8. Below that, only that item is rewritten, and only its text (plus its label: `tone` for barks, `register` for alerts). The script refuses anything else: another item touched, another field changed, a text identical to the failing one, a refined file the validator rejects.
- **Limit:** at most 2 refinement passes. If items are still below 8 after that, the loop stops and writes `escalation.md` for the owner instead of looping on.
- **Deterministic first:** the validator and the script's film-catalogue check (F4) run before and independently of any model judgment.

**Result of the run** (28 lines: 12 barks, 8 alerts, 8 upgrade flavors):

| Round | Items below 8 | Action |
|---|---|---|
| 0 (generated, no vault) | 12 of 28 | refine 12 items |
| 1 | 2 of 28 | refine 2 items |
| 2 | 1 of 28 | **escalated** to the owner |

27 of 28 lines end at 8 or above on every dimension. The remaining one is reported, not hidden: `barks/vesna_upgrade_offer_01` ("Pick one. Try not to overthink." teases the player's choice instead of Rook's plans, T2). It **passed in rounds 0 and 1** and failed in round 2: the evaluator is not perfectly stable between passes. The full table of the 14 corrections is in [`report.md`](../../Tools/style/runs/20261020-a6-run3/report.md); every score file and reason is next to it.

**The owner corrected the evaluator's rule.** Run 2 applied rule T3 as it was written ("no real film line, even paraphrased") and rewrote 24 lines, including cheeky winks such as "Mama needs a new gun". The owner read the report and disagreed: those lines are what he wants (a famous line clearly twisted into the Rigs' world); only a word-for-word or barely changed line is a fault. He rewrote T3 in the vault, left the judgment of "clearly twisted" to the evaluator, and run 3 restarted from the same 28 generated lines. His four lines that run 2 had flagged (`rook_level_up_01`, `rook_player_damaged_01`, `rook_run_won_01`, `rook_bonus_chest_opened_01`) pass untouched in run 3, and the film-line violations dropped from 13 to 5 (`upgrade_texts`, all "line plus a tail", e.g. "Say hello to my little friend, but louder.").

## Three before/after examples

The "before" content is real: 28 lines written by a sonnet agent with a loose brief and **no vault access** (the brief asked Rook to quote action-movie one-liners, which is why film catchphrases dominate the violations). Raw output: [`generator-output.json`](../../Tools/content/logs/a6-baseline/generator-output.json).

| | Violation class | Item | Before (scores) | Reason from the evaluator | After (final scores) |
|---|---|---|---|---|---|
| 1 | **Reference** (T3, a famous line left almost intact) | `upgrade_texts/upg_damage` | "Say hello to my little friend, but louder." (voice 8, lore 8, **reference 5**) | "Keeps Scarface's 'Say hello to my little friend' word for word and only tacks on 'but louder'; barely changed, so it is still the same line." | "Say hello to my little drill bit." (9, 9, 8: "a one-noun twist into Rigs jargon, same pattern as the owner's 'new gun' example") |
| 2 | **Voice** (T6, company jokes at intruders, wrong register) | `alerts/kv_extraction_ready_01` | "Your premium departure experience awaits. Thank you for visiting!" (**voice 3**, lore 6, reference 9), register *propaganda* | "The shuttle is the Rigs' extraction craft, so this is aimed at intruders and must be bureaucratic, not propaganda (label and text both wrong)." | "Unregistered craft detected on claim. Departure fees will be invoiced." (9, 9, 9), register *bureaucratic* |
| 3 | **Format** (F3, invented number) | `alerts/kv_console_dropped_02` | "Unauthorized console retrieval will be logged under Form 27-B." (**format 0**: the validator, "number 27 does not appear in an excerpt"; the model scores were 9, 8, 8) | Not the evaluator's: the deterministic check fails the item alone, which is why format is not left to a model. | "Network breach recorded. Responsible parties will be invoiced." (9, 9, 8, format 10) |

A fourth class, lore (T9), is in the same run: `alerts/kv_horde_incoming_01` "Compliance is mandatory and appreciated." (lore 6, an indirect order to the player) became "Security units released across the claim. Unauthorized personnel will be processed." (9, 9, 9). The refiner's retrieval log for each correction (query, vault nodes, verbatim excerpts) is in `refined/round1/`.


## Where it runs

The loop runs in the text pipeline right after the text agent writes a content file and before the validated file is imported into the game's content tables (`/style-loop` command, [`.claude/commands/style-loop.md`](../../.claude/commands/style-loop.md)).

## How to run it

```
/style-loop <run-id> <stem>:<content file>:<log file> ...
node Tools/style/style_loop.js --selftest        # 12 contract checks, no model needed
node Tools/style/style_loop.js init|check|gate|apply|report <runDir> ...
```

Files: [`Tools/style/style_loop.js`](../../Tools/style/style_loop.js), evaluator mode in [`.claude/agents/style-guide.md`](../../.claude/agents/style-guide.md), vault nodes `style-guide-text` and `style-loop`. Run folder: [`Tools/style/runs/20261020-a6-run3/`](../../Tools/style/runs/20261020-a6-run3/) (scores, gates, refined files and their retrieval logs, `report.md`, `escalation.md`).

## How the run was driven, and what went wrong first

- Scripts cannot call a model in this environment, so the orchestrating Claude session drove the loop and launched the evaluator and refiner agents; the script decides every pass, fail and merge. The score files are the evaluator's own output, unedited.
- **Runs 1 and 2 are kept as history** ([run 1](../../Tools/style/runs/20261020-a6-run1/), [run 2](../../Tools/style/runs/20261020-a6-run2/)); the numbers in this README are run 3.
- **Run 1 was abandoned** ([`20261020-a6-run1/`](../../Tools/style/runs/20261020-a6-run1/)). It exposed two gaps: the evaluator let two film titles through that are not in the catalogue, and a refiner could not correct a wrong `register` label because only the text field was allowed to change. I added the deterministic F4 check, allowed the label to change with its line, and added three checks to the evaluator's definition (look every title up, compare label to text, suggestions must obey the rules), then started run 2 from the same 28 generated lines. Run 2 ended escalated with 2 lines left, but with the old T3 (see above), which is why run 3 exists.
- Refiners treat evaluator suggestions as hints: several suggestions broke a rule themselves (for example a film title missing from the catalogue) and were rewritten rather than applied.
- Only text lines were evaluated. The design and voice of Rook and Vesna, new film titles and any rule change stay with the owner; the loop never edits a rule.
