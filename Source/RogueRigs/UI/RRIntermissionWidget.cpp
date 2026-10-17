// Rogue Rigs - C++ base of the intermission screen (WBP_Intermission, task-206-04): level recap and the Continue button.

#include "UI/RRIntermissionWidget.h"

void URRIntermissionWidget::ShowRecap(const FRRIntermissionRecap& InRecap)
{
	Recap = InRecap;
	bHasRecap = true;
	OnRecapShown(Recap);
}

bool URRIntermissionWidget::Continue()
{
	URRRunTransitionSubsystem* Transition = URRRunTransitionSubsystem::Get(this);
	return Transition && Transition->Continue();
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
