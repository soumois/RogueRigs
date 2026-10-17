// Rogue Rigs - screen text: the texts of one screen (end, intermission, loading) read from screen_texts.json (text-content-files).

#pragma once

#include "CoreMinimal.h"
#include "RRScreenText.generated.h"

/**
 * The text slots of one screen item of screen_texts.json (sys-text-content). Title is always set on a loaded item; the other
 * slots are empty when the item does not define them. Returned by URRTextContentSubsystem::GetScreenText to the widgets.
 */
USTRUCT(BlueprintType)
struct ROGUERIGS_API FRRScreenText
{
	GENERATED_BODY()

	/** Item id (e.g. end_run_won). */
	UPROPERTY(BlueprintReadOnly, Category = "Text")
	FName Id;

	/** Main heading of the screen (1 to 4 words). */
	UPROPERTY(BlueprintReadOnly, Category = "Text")
	FText Title;

	/** Line under the title (3 to 12 words); empty when the item has none. */
	UPROPERTY(BlueprintReadOnly, Category = "Text")
	FText Subtitle;

	/** Body lines in file order (3 to 12 words each); empty when the item has none. */
	UPROPERTY(BlueprintReadOnly, Category = "Text")
	TArray<FText> Lines;

	/** Short labels in file order (1 to 4 words each, e.g. buttons or captions); empty when the item has none. */
	UPROPERTY(BlueprintReadOnly, Category = "Text")
	TArray<FText> Labels;
};
