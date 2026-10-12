---
id: wbp-health-status
description: "WBP_HealthStatus, HUD panel for health bar and text, GAME OVER and damage vignette, driven by miner events; open for its tree and graphs."
parent: blueprints-index
kind: doc
---
# WBP_HealthStatus

- asset_path: /Game/RogueRigs/UI/WBP_HealthStatus
- parent_class: URRHUDPanelWidget (/Script/RogueRigs.RRHUDPanelWidget, [[sys-hud-events]])
- purpose: shows health, GAME OVER and the damage vignette; a sub-widget of [[wbp-hud]], which only places it (child index 0, variable HealthStatus, HitTestInvisible, anchors 0,0 to 1,1, zero offsets, z 0: it fills the screen so the moved elements keep their old absolute positions).
- components: CanvasPanel Root; DamageVignette (Image, brush M_HUDVignette, fills the root, HitTestInvisible, RenderOpacity 0.001); HealthBar (ProgressBar, 20,232, 280 x 28, red fill 0.9/0.05/0.05); HealthText (TextBlock Roboto Bold 24, 20,264, 280 x 30, "0 / 0"); GameOverText (TextBlock "GAME OVER", Roboto Bold 72, red, anchor 0.5/0.5, auto size, Collapsed). All four are variables (same values as they had in WBP_HUD).
- variables: HitFlash, LowHealthAlpha, HitHoldRemaining, LastFadeTime (float); HitHoldTime (float, 0.15, instance editable).
- events: hook BindToMiner(Miner, PreviousMiner): UnbindMiner(Previous) (unbinds all OnHealthChanged / OnDamaged listeners of a valid previous miner), then for a valid miner assigns OnHealthChanged and OnDamaged and calls ShowHealth(GetCurrentHealth, GetMaxHealth); null miner shows 0 / 0. Hook BindToGameMode(GameMode, PreviousGameMode): UnbindGameMode(Previous), assigns OnGameOver. Handlers (editor names with the "_1" / "_2" suffix from MCP authoring): OnHealthChanged_Event_1(Current, Max) calls ShowHealth; OnDamaged_Event_2(Amount, HealthFraction) sets HitFlash = 0.4 + 0.4 * (1 - HealthFraction), HitHoldRemaining = HitHoldTime, ApplyVignette, StartFade; OnGameOver_Event_1 shows GameOverText. Destruct unbinds miner and game mode and clears the fade timer.
- functions: ShowHealth(Current, Max): bar percent = Current / max(Max, 1) when Max > 0 else 0, text "round(Current) / round(Max)", LowHealthAlpha = 0.4 - percent when percent < 0.4 else 0, ApplyVignette. ApplyVignette: vignette RenderOpacity = clamp(HitFlash + LowHealthAlpha, 0.001, 0.9) (the 0.001 floor replaces the C++ KeepVignetteWarm, which only finds widgets of the HUD tree). StartFade: starts the looping timer FadeStep (0.016 s, once per frame); LastFadeTime is no longer used. FadeStep: dt = GetWorldDeltaSeconds (task-104-01: the old dt = game time minus LastFadeTime read the pure LastFadeTime getter after SetLastFadeTime had run, so dt was always 0 and the flash never faded); while HitHoldRemaining > 0 the flash holds and the hold drops by dt, else HitFlash decays 2 per s; ApplyVignette; clears its timer when flash and hold are 0.
- tick: none. The only per-frame work is the FadeStep timer, running about 0.5 s after a hit; the low-health tint follows OnHealthChanged. A hit now always restarts the hold (the old PrevHitFlash comparison is gone).
- logic_in_bp: display only (show values and timing of a fade).
- assets: M_HUDVignette (material, see [[wbp-hud]]).
- owner_agent: agent-bp
- verified: task-104-01 fix compiled 0 errors, saved, FadeStep read back; the headless test PressureVignetteOnHit is rerun by the Orchestrator. Earlier: compiled with warnings as errors, saved, graphs read back. Not seen on screen or in PIE (nobody sees the screen); the fade timing and the binding at run time are unverified. Known side effects: C++ KeepVignetteWarm and the debug command rr.Debug.DamageMiner / rr.Debug.Shots (RRVisualCheckCommands.cpp LogVignetteState) look up DamageVignette in the HUD tree and no longer find it; UnbindAll on OnGameOver would also remove any other Blueprint listener.

## Relations
- [[blueprints-index]] — parent index.
- [[wbp-hud]] — places it as a child instance.
- [[sys-hud-events]] — the base class, miner events and game-over event it binds.
