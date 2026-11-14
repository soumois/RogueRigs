// Rogue Rigs - C++ base of the end-of-level stats screen (WBP_Intermission, task-206-04, task-317-01): level recap and the Next
// level button.

#include "UI/RRIntermissionWidget.h"

#include "Run/RRIntermissionShopSubsystem.h"
#include "UI/RRButtonHover.h"

URRIntermissionWidget::URRIntermissionWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EntranceParts.TitleGroup = {TEXT("Title"), TEXT("TitleBarL"), TEXT("TitleBarR"), TEXT("Subtitle"), TEXT("SubtitleLineL"), TEXT("SubtitleLineR")};
	EntranceParts.FrameGroup = {TEXT("Frame"), TEXT("OreRow"), TEXT("DividerH"), TEXT("DividerV"), TEXT("KillsLine"), TEXT("DamageLine"),
		TEXT("KillsRows"), TEXT("DamageRows"), TEXT("KillsLabel"), TEXT("DamageLabel")};
	EntranceParts.Buttons = {TEXT("ContinueButton")};
}

URRIntermissionShopSubsystem* URRIntermissionWidget::GetShop() const
{
	return URRIntermissionShopSubsystem::Get(this);
}

void URRIntermissionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	URRButtonHoverSubsystem::AttachNamed(this, {TEXT("ContinueButton")});
	Entrance = NewObject<URRScreenEntrance>(this);
	Entrance->Setup(this, EntranceParts);
}

void URRIntermissionWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (Entrance)
	{
		Entrance->Tick(InDeltaTime);
	}
}

bool URRIntermissionWidget::SkipEntrance()
{
	return Entrance && Entrance->Skip();
}

bool URRIntermissionWidget::IsEntrancePlaying() const
{
	return Entrance && Entrance->IsPlaying();
}

void URRIntermissionWidget::NativeConstruct()
{
	if (URRIntermissionShopSubsystem* Shop = GetShop())
	{
		Shop->OnShopChanged.AddUniqueDynamic(this, &URRIntermissionWidget::HandleShopChanged);
	}
	Super::NativeConstruct();
}

void URRIntermissionWidget::NativeDestruct()
{
	if (URRIntermissionShopSubsystem* Shop = GetShop())
	{
		Shop->OnShopChanged.RemoveDynamic(this, &URRIntermissionWidget::HandleShopChanged);
	}
	if (Entrance)
	{
		Entrance->Restore();
	}
	Super::NativeDestruct();
}

void URRIntermissionWidget::HandleShopChanged()
{
	OnShopRefreshed();
}

void URRIntermissionWidget::ShowRecap(const FRRIntermissionRecap& InRecap)
{
	Recap = InRecap;
	bHasRecap = true;
	OnRecapShown(Recap);
	if (Entrance)
	{
		Entrance->Start();
	}
}

bool URRIntermissionWidget::NextLevel()
{
	URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(this);
	return Transition && Transition->NextLevel();
}

bool URRIntermissionWidget::Continue()
{
	URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(this);
	if (!Transition)
	{
		return false;
	}
	return Transition->IsInStatsScreen() ? Transition->NextLevel() : Transition->Continue();
}

FRRIntermissionRecap URRIntermissionWidget::GetRecap() const
{
	if (bHasRecap)
	{
		return Recap;
	}
	const URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(this);
	return Transition ? Transition->GetIntermissionRecap() : FRRIntermissionRecap();
}
