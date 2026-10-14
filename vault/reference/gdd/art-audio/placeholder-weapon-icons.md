---
id: placeholder-weapon-icons
description: "TEMPORARY placeholder HUD icons (padlock, burst rifle, shotgun, turret launcher) validated by the owner on 2026-10-12; replace when the final weapons are decided."
parent: art-audio-index
kind: note
---
# Placeholder weapon icons (temporary)

**Status: temporary.** These four images are placeholders for the weapon slot HUD until the weapons are decided (owner 2026-10-12). Do not treat them as final art and do not build on their design.

| Icon | File in `vault/assets/` | Candidate (run in `Art/Source/weapon-icons/`) |
|---|---|---|
| Padlock (locked slot) | `placeholder-icon-padlock.png` | `20261012-170024-weapon-icons-1.png` (owner pick) |
| Burst rifle | `placeholder-icon-burst-rifle.png` | `20261012-170141-weapon-icons-2.png` |
| Shotgun | `placeholder-icon-shotgun.png` | `20261012-170244-weapon-icons-1.png` |
| Turret launcher | `placeholder-icon-turret-launcher.png` | `20261012-170401-weapon-icons-2.png` |

- Made by the art agent ([[agent-art]]) on a flat magenta background; reviewed by the critic (8 of 8 pass, `Tools/content/logs/20261012-weapon-icons.review.json`); the owner chose the padlock variant 1, the others follow the critic's picks. The Orchestrator copied them to `vault/assets/` on the owner's instruction.
- The weapon shapes are generic gun anatomy chosen by the agent: the weapon nodes give no visual detail beyond colours.
- Where to replace: textures /Game/RogueRigs/UI/Icons/T_Icon_* (imported from Art/UI/Icons/); the weapon-name to texture mapping is the function GetWeaponIcon in WBP_Weapons (the padlock is the brush of LockImage0..3). A new weapon needs a new branch in GetWeaponIcon. Since task-168-04 the same mapping also exists in C++ (`URRIconSettings` `WeaponIcons` / `LockedSlotIcon`, read by `URRIconLibrary`, [[sys-upgrades]]), meant to become the single place once WBP_Weapons calls it; until then, a change goes in both.
- Use: the padlock is a 256 x 256 transparent PNG (task-108-02); the three weapon icons are trimmed PNGs that keep their aspect ratio (task-109-01, longest side 256: rifle 256 x 105, shotgun 256 x 98, launcher 256 x 170), no longer square and shown in the weapon slots (task-106-02, [[wbp-weapons]]). When the final weapons exist, replace the icons in the single mapping place of WBP_Weapons and delete these files.

## Relations
- [[art-audio-index]] — parent index.
- [[agent-art]] — the agent that produced the candidates.
- [[wbp-weapons]] — the HUD widget that shows them.
- [[color-code]] — the colours the icons follow.
