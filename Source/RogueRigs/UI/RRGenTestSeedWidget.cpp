// Rogue Rigs - seed line of the generator test map (task-494-01).

#include "UI/RRGenTestSeedWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

void URRGenTestSeedWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (WidgetTree && !WidgetTree->RootWidget)
	{
		LineText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("LineText"));
		FSlateFontInfo Font = LineText->GetFont();
		Font.Size = 20;
		Font.OutlineSettings.OutlineSize = 2;
		Font.OutlineSettings.OutlineColor = FLinearColor::Black;
		LineText->SetFont(Font);
		LineText->SetJustification(ETextJustify::Center);
		// Transparent border: only its padding, which keeps the line off the top edge of the screen.
		UBorder* Frame = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Frame"));
		Frame->SetBrushColor(FLinearColor::Transparent);
		Frame->SetPadding(FMargin(0.f, 16.f, 0.f, 0.f));
		Frame->SetContent(LineText);
		WidgetTree->RootWidget = Frame;
	}
	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void URRGenTestSeedWidget::SetLine(const FString& Text, const FLinearColor& Color)
{
	if (LineText)
	{
		LineText->SetText(FText::FromString(Text));
		LineText->SetColorAndOpacity(FSlateColor(Color));
	}
}

FString URRGenTestSeedWidget::GetLine() const
{
	return LineText ? LineText->GetText().ToString() : FString();
}
