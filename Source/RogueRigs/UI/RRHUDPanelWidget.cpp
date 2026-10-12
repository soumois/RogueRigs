// Rogue Rigs - base class of the HUD sub-widgets (panels placed in WBP_HUD): receives the miner, run flow, spawner, game mode and the HUD root itself from the HUD root once each.

#include "UI/RRHUDPanelWidget.h"

#include "Enemies/RREnemySpawner.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Player/RRGameMode.h"
#include "Player/RRMiner.h"
#include "UI/RRHUDWidget.h"

void URRHUDPanelWidget::SetMiner(ARRMiner* Miner)
{
	ARRMiner* Previous = BoundMiner.Get();
	if (bMinerSet && Previous == Miner)
	{
		return;
	}
	bMinerSet = true;
	BoundMiner = Miner;
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
	BindToRunFlow(RunFlow, Previous);
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
