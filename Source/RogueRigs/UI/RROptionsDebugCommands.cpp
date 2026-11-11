// Rogue Rigs - options screen debug console command (task-505-02, compiled out of shipping):
// rr.Options dump|tab <sound|graphics>|step <Row> <dir>|reset|back on the open options screen (same functions as its buttons).

#include "CoreMinimal.h"

#if !UE_BUILD_SHIPPING

#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "Menu/RRMainMenuWidget.h"
#include "RogueRigs.h"
#include "UI/RROptionsWidget.h"
#include "UObject/UObjectIterator.h"

namespace RROptionsDebug
{
	URROptionsWidget* FindOpenOptions(UWorld* World)
	{
		for (TObjectIterator<URROptionsWidget> It; It; ++It)
		{
			if (It->GetWorld() == World && It->IsInViewport())
			{
				return *It;
			}
		}
		return nullptr;
	}

	void Run(const TArray<FString>& Args, UWorld* World)
	{
		if (Args.Num() > 0 && Args[0].ToLower() == TEXT("open"))
		{
			for (TObjectIterator<URRMainMenuWidget> It; It; ++It)
			{
				if (It->GetWorld() == World && It->IsInViewport())
				{
					UE_LOG(LogRogueRigs, Log, TEXT("RROPTIONS open: %s"), It->OpenOptions() ? TEXT("accepted") : TEXT("refused"));
					return;
				}
			}
			UE_LOG(LogRogueRigs, Warning, TEXT("RROPTIONS open: no main menu."));
			return;
		}
		URROptionsWidget* Widget = FindOpenOptions(World);
		if (!Widget)
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("RROPTIONS: no options screen open."));
			return;
		}
		const FString Action = Args.Num() > 0 ? Args[0].ToLower() : TEXT("dump");
		const UEnum* RowEnum = StaticEnum<ERROptionRow>();
		if (Action == TEXT("tab") && Args.Num() > 1)
		{
			Widget->ShowTab(Args[1].ToLower() == TEXT("sound") ? ERROptionsTab::Sound : ERROptionsTab::Graphics);
		}
		else if (Action == TEXT("step") && Args.Num() > 2)
		{
			const int64 Row = RowEnum->GetValueByNameString(Args[1]);
			if (Row == INDEX_NONE)
			{
				UE_LOG(LogRogueRigs, Warning, TEXT("RROPTIONS: unknown row %s."), *Args[1]);
				return;
			}
			Widget->StepRow(ERROptionRow(Row), FCString::Atoi(*Args[2]));
		}
		else if (Action == TEXT("reset"))
		{
			Widget->ResetCurrentTab();
		}
		else if (Action == TEXT("back"))
		{
			Widget->Back();
			return;
		}
		FString Line;
		for (int32 Index = 0; Index < RowEnum->NumEnums() - 1; ++Index)
		{
			const ERROptionRow Row = ERROptionRow(RowEnum->GetValueByIndex(Index));
			Line += FString::Printf(TEXT("%s=%s; "), *RowEnum->GetNameStringByIndex(Index), *Widget->GetRowValueString(Row));
		}
		UE_LOG(LogRogueRigs, Log, TEXT("RROPTIONS tab=%s %s"), Widget->GetCurrentTab() == ERROptionsTab::Sound ? TEXT("Sound") : TEXT("Graphics"), *Line);
	}

	FAutoConsoleCommandWithWorldAndArgs Command(
		TEXT("rr.Options"),
		TEXT("Open options screen: rr.Options dump | tab sound|graphics | step <Row> <1|-1> | reset | back."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&Run),
		ECVF_Cheat);
}

#endif
