// Rogue Rigs - base class of the HUD sub-widgets (panels placed in WBP_HUD): receives the miner, run flow, spawner, game mode and the HUD root itself from the HUD root once each.

#include "UI/RRHUDPanelWidget.h"

#include "Enemies/RREnemySpawner.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Player/RRGameMode.h"
#include "Player/RRMiner.h"
#include "UI/RRButtonHover.h"
#include "UI/RRHUDWidget.h"
#include "Weapons/RRWeapon.h"

void URRHUDPanelWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	URRButtonHoverSubsystem::AttachNamed(this, HoverButtonNames);
}

void URRHUDPanelWidget::SetMiner(ARRMiner* Miner)
{
	ARRMiner* Previous = BoundMiner.Get();
	if (bMinerSet && Previous == Miner)
	{
		return;
	}
	bMinerSet = true;
	BoundMiner = Miner;
	NativeBindToMiner(Miner, Previous);
	BindToMiner(Miner, Previous);
}

void URRHUDPanelWidget::SetRunFlow(URRRunFlowSubsystem* RunFlow)
{
	URRRunFlowSubsystem* Previous = BoundRunFlow.Get();
	if (bRunFlowSet && Previous == RunFlow)
	{
		return;
	}
	bRunFlowSet = true;
	BoundRunFlow = RunFlow;
	// Horde gauge (task-544): bound natively for every panel, so a Blueprint panel only implements OnHordeGaugeChanged.
	if (Previous)
	{
		Previous->OnHordeGaugeChanged.RemoveDynamic(this, &URRHUDPanelWidget::HandleHordeGaugeChanged);
	}
	if (RunFlow)
	{
		RunFlow->OnHordeGaugeChanged.AddUniqueDynamic(this, &URRHUDPanelWidget::HandleHordeGaugeChanged);
	}
	BindToRunFlow(RunFlow, Previous);
	if (RunFlow)
	{
		OnHordeGaugeChanged(RunFlow->GetHordeGaugeState(), RunFlow->GetHordeGaugeFill());
	}
}

void URRHUDPanelWidget::HandleHordeGaugeChanged(ERRHordeGaugeState State, float Fill)
{
	OnHordeGaugeChanged(State, Fill);
}

void URRHUDPanelWidget::SetSpawner(ARREnemySpawner* Spawner)
{
	ARREnemySpawner* Previous = BoundSpawner.Get();
	if (bSpawnerSet && Previous == Spawner)
	{
		return;
	}
	bSpawnerSet = true;
	BoundSpawner = Spawner;
	BindToSpawner(Spawner, Previous);
}

void URRHUDPanelWidget::SetGameMode(ARRGameMode* GameMode)
{
	ARRGameMode* Previous = BoundGameMode.Get();
	if (bGameModeSet && Previous == GameMode)
	{
		return;
	}
	bGameModeSet = true;
	BoundGameMode = GameMode;
	BindToGameMode(GameMode, Previous);
}

void URRHUDPanelWidget::BindToMiner_Implementation(ARRMiner* Miner, ARRMiner* PreviousMiner)
{
	++MinerBindCount;
}

void URRHUDPanelWidget::BindToRunFlow_Implementation(URRRunFlowSubsystem* RunFlow, URRRunFlowSubsystem* PreviousRunFlow)
{
	++RunFlowBindCount;
}

void URRHUDPanelWidget::BindToSpawner_Implementation(ARREnemySpawner* Spawner, ARREnemySpawner* PreviousSpawner)
{
	++SpawnerBindCount;
}

void URRHUDPanelWidget::SetHUD(URRHUDWidget* HUD)
{
	URRHUDWidget* Previous = BoundHUD.Get();
	if (!HUD || Previous == HUD)
	{
		return;
	}
	BoundHUD = HUD;
	BindToHUD(HUD, Previous);
}

void URRHUDPanelWidget::BindToHUD_Implementation(URRHUDWidget* HUD, URRHUDWidget* PreviousHUD)
{
	++HUDBindCount;
}
