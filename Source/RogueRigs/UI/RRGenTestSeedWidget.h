// Rogue Rigs - seed line of the generator test map (task-494-01): one native text at the top centre of the screen, no Blueprint.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RRGenTestSeedWidget.generated.h"

class UTextBlock;

/**
 * Native text line (dev test map plumbing, cpp-bp-split): the tree (a transparent UBorder padding one outlined UTextBlock 16 px from the top) is built in NativeOnInitialized.
 * ARRGenTestGameMode adds it to player 0's viewport anchored at the top centre and calls SetLine on each generation step.
 * Not hit-testable.
 */
UCLASS()
class ROGUERIGS_API URRGenTestSeedWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Shows Text in Color. */
	void SetLine(const FString& Text, const FLinearColor& Color);

	/** Text shown now (tests, logs). */
	FString GetLine() const;

protected:
	/** Builds the tree: a transparent border with top padding around an outlined text block; not hit-testable. */
	virtual void NativeOnInitialized() override;

	/** The text line. */
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> LineText;
};
