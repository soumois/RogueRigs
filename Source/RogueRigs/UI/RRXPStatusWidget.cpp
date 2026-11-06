// Rogue Rigs - XP status HUD panel (parent of WBP_XPStatus): drives the XP bar fill and a pale ghost fill from the miner progression events; on a gain the ghost shows the new XP at once and the fill catches up after a short delay.

#include "UI/RRXPStatusWidget.h"

#include "Components/ProgressBar.h"
#include "Engine/World.h"
#include "Player/RRMiner.h"
#include "Progression/RRProgressionComponent.h"

namespace RRXPStatus
{
	/** XP fraction of a level, clamped 0..1 (0 when the need is 0). */
	float Fraction(float Current, float Needed)
	{
		return Needed > 0.f ? FMath::Clamp(Current / Needed, 0.f, 1.f) : 0.f;
	}
}

void URRXPStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// A Blueprint Destruct may unbind the progression: bind again when the widget comes back.
	if (ARRMiner* Miner = GetMiner())
	{
		BindProgression(Miner->GetProgression());
	}
	ApplyXPBarVisuals();
}

void URRXPStatusWidget::NativeDestruct()
{
	StopXPBarUpdate();
	Super::NativeDestruct();
}

void URRXPStatusWidget::BindProgression(URRProgressionComponent* Progression)
{
	if (Progression)
	{
		Progression->OnXPChanged.AddUniqueDynamic(this, &URRXPStatusWidget::HandleXPChanged);
		Progression->OnLevelUp.AddUniqueDynamic(this, &URRXPStatusWidget::HandleLevelUp);
	}
}

void URRXPStatusWidget::NativeBindToMiner(ARRMiner* Miner, ARRMiner* PreviousMiner)
{
	if (URRProgressionComponent* Previous = PreviousMiner ? PreviousMiner->GetProgression() : nullptr)
	{
		Previous->OnXPChanged.RemoveDynamic(this, &URRXPStatusWidget::HandleXPChanged);
		Previous->OnLevelUp.RemoveDynamic(this, &URRXPStatusWidget::HandleLevelUp);
	}
	URRProgressionComponent* Progression = Miner ? Miner->GetProgression() : nullptr;
	BindProgression(Progression);
	TargetFraction = Progression ? RRXPStatus::Fraction(Progression->GetXP(), Progression->GetXPToNextLevel()) : 0.f;
	ShownLevel = Progression ? Progression->GetLevel() : 0;
	StopXPBarUpdate();
	SnapXPBar();
	ApplyXPBarVisuals();
}

void URRXPStatusWidget::HandleLevelUp(int32 NewLevel)
{
	bLevelUpReceived = true;
}

void URRXPStatusWidget::HandleXPChanged(float Current, float Needed, int32 Level)
{
	TargetFraction = RRXPStatus::Fraction(Current, Needed);
	const bool bLevelUp = bLevelUpReceived;
	bLevelUpReceived = false;
	if (!bLevelUp && Level != ShownLevel)
	{
		// Level set without a level-up (run carry-over restore): show it at once.
		ShownLevel = Level;
		SnapXPBar();
		ApplyXPBarVisuals();
		return;
	}
	ShownLevel = Level;
	if (bLevelUp)
	{
		// The ghost goes to the end; the remainder shows once the fill reached it and the bar emptied. Several levels = one pass.
		bLevelUpPending = true;
	}
	GhostFraction = bLevelUpPending ? 1.f : TargetFraction;
	FillFraction = FMath::Min(FillFraction, GhostFraction);
	DelayRemaining = CatchUpDelay;
	ApplyXPBarVisuals();
	StartXPBarUpdate();
}

void URRXPStatusWidget::SnapXPBar()
{
	GhostFraction = TargetFraction;
	FillFraction = TargetFraction;
	DelayRemaining = 0.f;
	bLevelUpPending = false;
	bLevelUpReceived = false;
}

void URRXPStatusWidget::AdvanceXPBar(float DeltaSeconds)
{
	if (DelayRemaining > 0.f)
	{
		DelayRemaining = FMath::Max(0.f, DelayRemaining - DeltaSeconds);
	}
	else
	{
		FillFraction += (GhostFraction - FillFraction) * FMath::Min(1.f, CatchUpRate * DeltaSeconds);
		if (FMath::Abs(GhostFraction - FillFraction) < CatchUpSnapThreshold)
		{
			FillFraction = GhostFraction;
		}
		if (bLevelUpPending && FillFraction >= 1.f)
		{
			// The bar is full: it empties, then the remainder shows the same way (ghost at once, fill after the delay).
			bLevelUpPending = false;
			FillFraction = 0.f;
			GhostFraction = TargetFraction;
			DelayRemaining = GhostFraction > 0.f ? CatchUpDelay : 0.f;
		}
	}
	FillFraction = FMath::Min(FillFraction, GhostFraction);
	ApplyXPBarVisuals();
}

bool URRXPStatusWidget::IsXPBarSettled() const
{
	return !bLevelUpPending && FillFraction == GhostFraction && DelayRemaining <= 0.f;
}

void URRXPStatusWidget::StartXPBarUpdate()
{
	if (bXPBarUpdateRunning)
	{
		return;
	}
	UWorld* World = GetWorld();
	if (!World)
	{
		// No world to tick in: show the end state at once.
		SnapXPBar();
		ApplyXPBarVisuals();
		return;
	}
	bXPBarUpdateRunning = true;
	XPBarUpdateTimer = World->GetTimerManager().SetTimerForNextTick(this, &URRXPStatusWidget::UpdateXPBar);
}

void URRXPStatusWidget::StopXPBarUpdate()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(XPBarUpdateTimer);
	}
	XPBarUpdateTimer.Invalidate();
	bXPBarUpdateRunning = false;
}

void URRXPStatusWidget::UpdateXPBar()
{
	bXPBarUpdateRunning = false;
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	AdvanceXPBar(World->GetDeltaSeconds());
	if (!IsXPBarSettled())
	{
		StartXPBarUpdate();
	}
}

void URRXPStatusWidget::ApplyXPBarVisuals()
{
	if (XPBar)
	{
		XPBar->SetPercent(FillFraction);
	}
	if (XPGhostBar)
	{
		XPGhostBar->SetPercent(GhostFraction);
		XPGhostBar->SetFillColorAndOpacity(GhostColor);
	}
}
