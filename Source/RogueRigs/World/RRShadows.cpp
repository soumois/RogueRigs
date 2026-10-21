// Rogue Rigs - shadow casting rule shared by every non-terrain actor (task-263-01): only the grid's rocks cast shadows.

#include "World/RRShadows.h"

#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"

void RRShadows::DisableCastShadows(const AActor* Actor)
{
	if (!Actor)
	{
		return;
	}
	TInlineComponentArray<UPrimitiveComponent*> Primitives(Actor);
	for (UPrimitiveComponent* Primitive : Primitives)
	{
		if (Primitive->CastShadow)
		{
			Primitive->SetCastShadow(false);
		}
	}
}
