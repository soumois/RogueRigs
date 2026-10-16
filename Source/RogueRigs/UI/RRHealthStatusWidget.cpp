// Rogue Rigs - health status HUD panel (future parent of WBP_HealthStatus): drives the health bar and text from the miner health events, with a smooth drain of the shown fill, a damage chunk behind it, a hit flash of the fill, a hit shake and a hit punch of the bar root, and a fill colour that follows the health left.

#include "UI/RRHealthStatusWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "Engine/World.h"
#include "Player/RRMiner.h"

void URRHealthStatusWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// The pivot belongs to the class, not to the Blueprint: the punch scales the bar root around its centre.
	if (HealthBarRoot)
	{
		HealthBarRoot->SetRenderTransformPivot(BarRootPivot);
	}
}

void URRHealthStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// The Blueprint Destruct graph unbinds every listener of the miner: bind again when the widget comes back.
	if (ARRMiner* Miner = GetMiner())
	{
		Miner->OnHealthChanged.AddUniqueDynamic(this, &URRHealthStatusWidget::HandleHealthChanged);
		Miner->OnDamaged.AddUniqueDynamic(this, &URRHealthStatusWidget::HandleDamaged);
	}
	ApplyBarVisuals();
}

void URRHealthStatusWidget::NativeDestruct()
{
	StopBarUpdate();
	Super::NativeDestruct();
}

void URRHealthStatusWidget::NativeBindToMiner(ARRMiner* Miner, ARRMiner* PreviousMiner)
{
	if (PreviousMiner)
	{
		PreviousMiner->OnHealthChanged.RemoveDynamic(this, &URRHealthStatusWidget::HandleHealthChanged);
		PreviousMiner->OnDamaged.RemoveDynamic(this, &URRHealthStatusWidget::HandleDamaged);
	}
	if (Miner)
	{
		Miner->OnHealthChanged.AddUniqueDynamic(this, &URRHealthStatusWidget::HandleHealthChanged);
		Miner->OnDamaged.AddUniqueDynamic(this, &URRHealthStatusWidget::HandleDamaged);
		ShowHealth(Miner->GetCurrentHealth(), Miner->GetMaxHealth(), true);
	}
	else
	{
		ShowHealth(0.f, 0.f, true);
	}
}

void URRHealthStatusWidget::HandleHealthChanged(float Current, float Max)
{
	ShowHealth(Current, Max, false);
}

void URRHealthStatusWidget::HandleDamaged(float Amount, float HealthFraction)
{
	// OnHealthChanged runs first, so the chunk still holds the fraction shown before the hit. Each hit restarts its hold.
	// Hit effects of the later subtasks start here; the drain itself follows HandleHealthChanged.
	ChunkFraction = FMath::Max(ChunkFraction, ShownFraction);
	ChunkDelayRemaining = ChunkDelay;
	FlashAlpha = FlashStrength;
	ShakeTimeRemaining = ShakeDuration;
	RollShakeOffset();
	PunchScaleY = PunchScale;
	PunchVelocity = 0.f;
	// Draw the full flash, the first shake offset and the full punch this frame; the fade and the spring start on the next one.
	ApplyBarVisuals();
	StartBarUpdate();
}

void URRHealthStatusWidget::ShowHealth(float Current, float Max, bool bSnap)
{
	if (HealthText)
	{
		HealthText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), FMath::RoundToInt(Current), FMath::RoundToInt(Max))));
	}
	TargetFraction = Max > 0.f ? FMath::Clamp(Current / Max, 0.f, 1.f) : 0.f;
	if (bSnap)
	{
		SnapBarEffects();
	}
	// The fill colour follows the real fraction: draw it at once, the shown fraction glides in the per-frame update.
	ApplyBarVisuals();
	if (!bSnap && !FMath::IsNearlyEqual(ShownFraction, TargetFraction))
	{
		StartBarUpdate();
	}
}

void URRHealthStatusWidget::SnapBarEffects()
{
	ShownFraction = TargetFraction;
	ChunkFraction = ShownFraction;
	ChunkDelayRemaining = 0.f;
	FlashAlpha = 0.f;
	ShakeTimeRemaining = 0.f;
	ShakeOffset = FVector2D::ZeroVector;
	PunchScaleY = 1.f;
	PunchVelocity = 0.f;
}

void URRHealthStatusWidget::StartBarUpdate()
{
	if (bBarUpdateRunning)
	{
		return;
	}
	UWorld* World = GetWorld();
	if (!World)
	{
		// No world to tick in: show the end state at once.
		SnapBarEffects();
		ApplyBarVisuals();
		return;
	}
	bBarUpdateRunning = true;
	BarUpdateTimer = World->GetTimerManager().SetTimerForNextTick(this, &URRHealthStatusWidget::UpdateBarEffects);
}

void URRHealthStatusWidget::StopBarUpdate()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BarUpdateTimer);
	}
	BarUpdateTimer.Invalidate();
	bBarUpdateRunning = false;
}

void URRHealthStatusWidget::UpdateBarEffects()
{
	bBarUpdateRunning = false;
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	AdvanceBarEffects(World->GetDeltaSeconds());
	ApplyBarVisuals();
	if (!AreBarEffectsSettled())
	{
		StartBarUpdate();
	}
}

void URRHealthStatusWidget::AdvanceBarEffects(float DeltaSeconds)
{
	ShownFraction += (TargetFraction - ShownFraction) * FMath::Min(1.f, DrainRate * DeltaSeconds);
	if (FMath::Abs(TargetFraction - ShownFraction) < DrainSnapThreshold)
	{
		ShownFraction = TargetFraction;
	}

	// Damage chunk: holds during its delay, then shrinks to the shown fraction; a heal past it pulls it up.
	if (ChunkDelayRemaining > 0.f)
	{
		ChunkDelayRemaining = FMath::Max(0.f, ChunkDelayRemaining - DeltaSeconds);
	}
	else
	{
		ChunkFraction += (ShownFraction - ChunkFraction) * FMath::Min(1.f, ChunkRate * DeltaSeconds);
		if (FMath::Abs(ShownFraction - ChunkFraction) < DrainSnapThreshold)
		{
			ChunkFraction = ShownFraction;
		}
	}
	ChunkFraction = FMath::Max(ChunkFraction, ShownFraction);

	// Hit flash: linear fade back to the base fill colour.
	FlashAlpha = FMath::Max(0.f, FlashAlpha - FlashDecay * DeltaSeconds);

	// Hit shake: new random offset each frame, amplitude decaying linearly to 0; zero (identity) at the end.
	ShakeTimeRemaining = FMath::Max(0.f, ShakeTimeRemaining - DeltaSeconds);
	RollShakeOffset();

	// Hit punch: damped spring of the scale Y back to 1.
	AdvancePunch(DeltaSeconds);
}

void URRHealthStatusWidget::AdvancePunch(float DeltaSeconds)
{
	if (PunchScaleY == 1.f && PunchVelocity == 0.f)
	{
		return;
	}
	// Semi-implicit Euler in substeps no longer than PunchMaxSubstep: stable whatever the frame time; the substep count is capped.
	const float MaxSubstep = FMath::Max(PunchMaxSubstep, UE_KINDA_SMALL_NUMBER);
	const int32 SubstepCount = FMath::Clamp(FMath::CeilToInt(FMath::Max(0.f, DeltaSeconds) / MaxSubstep), 1, FMath::Max(1, PunchMaxSubsteps));
	const float Substep = FMath::Min(FMath::Max(0.f, DeltaSeconds) / SubstepCount, MaxSubstep);
	for (int32 Index = 0; Index < SubstepCount; ++Index)
	{
		const float Acceleration = -PunchStiffness * (PunchScaleY - 1.f) - PunchDamping * PunchVelocity;
		PunchVelocity += Acceleration * Substep;
		PunchScaleY += PunchVelocity * Substep;
	}
	// Settle to exactly 1 once both the distance and the speed are tiny.
	if (FMath::Abs(PunchScaleY - 1.f) < PunchSnapThreshold && FMath::Abs(PunchVelocity) < PunchVelocitySnapThreshold)
	{
		PunchScaleY = 1.f;
		PunchVelocity = 0.f;
	}
}

void URRHealthStatusWidget::RollShakeOffset()
{
	if (ShakeTimeRemaining <= 0.f || ShakeDuration <= 0.f)
	{
		ShakeTimeRemaining = 0.f;
		ShakeOffset = FVector2D::ZeroVector;
		return;
	}
	const float Amplitude = ShakeAmplitude * (ShakeTimeRemaining / ShakeDuration);
	ShakeOffset = FVector2D(FMath::FRandRange(-Amplitude, Amplitude), FMath::FRandRange(-Amplitude, Amplitude));
}

bool URRHealthStatusWidget::AreBarEffectsSettled() const
{
	return ShownFraction == TargetFraction && ChunkFraction == ShownFraction && ChunkDelayRemaining <= 0.f && FlashAlpha <= 0.f
		&& ShakeTimeRemaining <= 0.f && PunchScaleY == 1.f && PunchVelocity == 0.f;
}

FLinearColor URRHealthStatusWidget::GetBaseFillColor() const
{
	return EvaluateFillColorStops(TargetFraction);
}

FLinearColor URRHealthStatusWidget::EvaluateFillColorStops(float Fraction) const
{
	// Nearest stop at or below the fraction and nearest stop at or above it; the stops may be in any order.
	const FRRHealthColorStop* Lower = nullptr;
	const FRRHealthColorStop* Upper = nullptr;
	for (const FRRHealthColorStop& Stop : FillColorStops)
	{
		if (Stop.Fraction <= Fraction && (!Lower || Stop.Fraction > Lower->Fraction))
		{
			Lower = &Stop;
		}
		if (Stop.Fraction >= Fraction && (!Upper || Stop.Fraction < Upper->Fraction))
		{
			Upper = &Stop;
		}
	}
	if (!Lower && !Upper)
	{
		return FLinearColor::White;
	}
	if (!Lower || !Upper)
	{
		// Beyond the end stops: hold the colour of the nearest one (red at 20 % and below with the defaults).
		return Lower ? Lower->Color : Upper->Color;
	}
	const float Span = Upper->Fraction - Lower->Fraction;
	if (Span <= UE_KINDA_SMALL_NUMBER)
	{
		return Lower->Color;
	}
	return FMath::Lerp(Lower->Color, Upper->Color, (Fraction - Lower->Fraction) / Span);
}

FLinearColor URRHealthStatusWidget::GetCurrentFillColor() const
{
	return FMath::Lerp(GetBaseFillColor(), FlashColor, FlashAlpha);
}

FWidgetTransform URRHealthStatusWidget::GetBarRootTransform() const
{
	// Single place that composes the bar root transform: shake translation and punch scale Y (scale X stays 1).
	FWidgetTransform Transform;
	Transform.Translation = ShakeOffset;
	Transform.Scale = FVector2D(1.f, PunchScaleY);
	return Transform;
}

void URRHealthStatusWidget::ApplyBarVisuals()
{
	if (HealthBar)
	{
		HealthBar->SetPercent(ShownFraction);
		HealthBar->SetFillColorAndOpacity(GetCurrentFillColor());
	}
	if (DamageChunkBar)
	{
		DamageChunkBar->SetPercent(ChunkFraction);
		DamageChunkBar->SetFillColorAndOpacity(ChunkColor);
	}
	if (HealthBarRoot)
	{
		HealthBarRoot->SetRenderTransform(GetBarRootTransform());
	}
}
