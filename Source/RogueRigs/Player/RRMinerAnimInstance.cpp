// Rogue Rigs - anim instance of the miner: ground speed, move direction relative to the aim, blend speed and play rate for ABP_Miner.

#include "Player/RRMinerAnimInstance.h"

#include "RogueRigs.h"
#include "Components/MeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/World.h"
#include "Player/RRMiner.h"
#include "Weapons/RRWeapon.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "Animation/AnimSequenceBase.h"
#include "HAL/IConsoleManager.h"

namespace RRMinerAnimDebug
{
	/** rr.Debug.MinerAnim: 1 logs MoveX, MoveY and GroundSpeed of every miner anim instance at about 2 Hz. */
	static TAutoConsoleVariable<int32> CVarMinerAnim(
		TEXT("rr.Debug.MinerAnim"), 0,
		TEXT("1 logs the miner anim instance MoveX, MoveY and GroundSpeed to LogRogueRigs at about 2 Hz; 0 off."),
		ECVF_Cheat);

	/** Seconds between two debug log lines (about 2 Hz). Debug-only, not a gameplay tunable. */
	static constexpr double LogIntervalSeconds = 0.5;

	/** rr.Debug.MinerAnimForce "x y": forces MoveX=x, MoveY=y (length clamped to 1) and GroundSpeed = length * MaxWalkSpeed; empty = off. */
	static TAutoConsoleVariable<FString> CVarMinerAnimForce(
		TEXT("rr.Debug.MinerAnimForce"), TEXT(""),
		TEXT("\"x y\" (two floats) forces the miner anim instance MoveX=x, MoveY=y (length clamped to 1) and GroundSpeed = length * MaxWalkSpeed; empty = off."),
		ECVF_Cheat);

	/** Parses the force cvar; true with the clamped move vector when it holds two floats. */
	static bool TryGetForcedMove(FVector2D& OutMove)
	{
		const FString Value = CVarMinerAnimForce.GetValueOnGameThread();
		TArray<FString> Tokens;
		Value.ParseIntoArrayWS(Tokens);
		float X = 0.f;
		float Y = 0.f;
		if (Tokens.Num() != 2 || !LexTryParseString(X, *Tokens[0]) || !LexTryParseString(Y, *Tokens[1]))
		{
			return false;
		}
		OutMove = FVector2D(X, Y);
		if (OutMove.SizeSquared() > 1.0)
		{
			OutMove.Normalize();
		}
		return true;
	}

	/** rr.Debug.MinerAnimFreeze: 1 stops the miner animation time (mesh GlobalAnimRateScale 0) while the graph keeps evaluating; 0 restores. */
	static TAutoConsoleVariable<int32> CVarMinerAnimFreeze(
		TEXT("rr.Debug.MinerAnimFreeze"), 0,
		TEXT("1 freezes the miner animation time (mesh GlobalAnimRateScale 0; the anim graph keeps evaluating, so a new rr.Debug.MinerUpperPose sequence shows on the same frame); 0 restores the previous rate scale."),
		ECVF_Cheat);

	/** rr.Debug.MinerUpperPose "<name>": the player miner upper body plays /Game/RogueRigs/Characters/Miner/Anims/<name>; empty = class default. */
	static TAutoConsoleVariable<FString> CVarMinerUpperPose(
		TEXT("rr.Debug.MinerUpperPose"), TEXT(""),
		TEXT("\"<name>\" sets the player miner UpperBodySequence (upper body layer of ABP_Miner) to the anim sequence /Game/RogueRigs/Characters/Miner/Anims/<name> (e.g. A_Miner_UpperRifle_V1); empty restores the ABP_Miner class default; a missing asset logs a warning and keeps the current sequence."),
		ECVF_Cheat);

	/** Content folder searched by rr.Debug.MinerUpperPose. Debug-only, not a gameplay tunable. */
	static const TCHAR* const UpperPoseFolder = TEXT("/Game/RogueRigs/Characters/Miner/Anims/");

	/** rr.Debug.RightHandOffset "X Y Z [Pitch Yaw Roll]": live right hand offset of the player miner in the WeaponSocket_R frame; empty = class defaults. */
	static TAutoConsoleVariable<FString> CVarRightHandOffset(
		TEXT("rr.Debug.RightHandOffset"), TEXT(""),
		TEXT("\"X Y Z\" or \"X Y Z Pitch Yaw Roll\" moves and turns the player miner right hand on the rifle, on top of the upper body pose: the rifle stays in place (it still follows the hand in the animations). ")
		TEXT("Location in cm, rotation in degrees, in the WeaponSocket_R frame: X toward the rifle muzzle, Y side, Z rifle top; rotation pivot = the socket point (the grip). ")
		TEXT("Overrides RightHandOffsetLocation / RightHandOffsetRotation of ABP_Miner; empty (\"\") restores the ABP_Miner class defaults; a bad value logs a warning and changes nothing. ")
		TEXT("Each apply logs the values to type into the ABP_Miner class defaults."),
		ECVF_Cheat);

	/** Parses "X Y Z" or "X Y Z Pitch Yaw Roll"; false (outputs untouched) on any other token count or a non-float token. */
	static bool TryParseRightHandOffset(const FString& Value, FVector& OutLocation, FRotator& OutRotation)
	{
		TArray<FString> Tokens;
		Value.ParseIntoArrayWS(Tokens);
		if (Tokens.Num() != 3 && Tokens.Num() != 6)
		{
			return false;
		}
		double Numbers[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
		for (int32 Index = 0; Index < Tokens.Num(); ++Index)
		{
			if (!LexTryParseString(Numbers[Index], *Tokens[Index]))
			{
				return false;
			}
		}
		OutLocation = FVector(Numbers[0], Numbers[1], Numbers[2]);
		OutRotation = FRotator(Numbers[3], Numbers[4], Numbers[5]);
		return true;
	}

	/** One-line text of a relative transform for the RightHandOffset log: location (cm), rotation (deg), scale. */
	static FString DescribeRelativeTransform(const FTransform& Transform)
	{
		const FVector Location = Transform.GetLocation();
		const FRotator Rotation = Transform.Rotator();
		const FVector Scale = Transform.GetScale3D();
		return FString::Printf(TEXT("(Loc=(X=%.3f,Y=%.3f,Z=%.3f) Rot=(Pitch=%.3f,Yaw=%.3f,Roll=%.3f) Scale=(X=%.3f,Y=%.3f,Z=%.3f))"),
			Location.X, Location.Y, Location.Z, Rotation.Pitch, Rotation.Yaw, Rotation.Roll, Scale.X, Scale.Y, Scale.Z);
	}
}

void URRMinerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	const APawn* PawnOwner = TryGetPawnOwner();
	const FVector Velocity = PawnOwner ? PawnOwner->GetVelocity() : FVector::ZeroVector;
	const FVector GroundVelocity(Velocity.X, Velocity.Y, 0.0);

	GroundSpeed = static_cast<float>(GroundVelocity.Size());

	if (PawnOwner && !FMath::IsNearlyZero(GroundSpeed))
	{
		// Velocity in the actor yaw frame: X forward (the aim), Y right. Same math as UKismetAnimationLibrary::CalculateDirection.
		const FRotator YawRotation(0.0, PawnOwner->GetActorRotation().Yaw, 0.0);
		const FVector LocalVelocity = YawRotation.UnrotateVector(GroundVelocity);
		Direction = static_cast<float>(FMath::RadiansToDegrees(FMath::Atan2(LocalVelocity.Y, LocalVelocity.X)));
	}
	else
	{
		Direction = 0.f;
	}

	RunSpeedForDirection = ComputeRunSpeedForDirection(Direction);
	// task-386-16: the simple blend space plays the clips at their authored rate, the Q4 clamp rule is off for now.
	LocomotionPlayRate = 1.f;
	BlendSpeed = GroundSpeed / LocomotionPlayRate;

	// Move vector (task-386-16): ground velocity in the actor yaw frame (the aim) over MaxWalkSpeed, clamped to length 1.
	const ACharacter* CharacterOwner = Cast<ACharacter>(PawnOwner);
	const UCharacterMovementComponent* MovementComponent = CharacterOwner ? CharacterOwner->GetCharacterMovement() : nullptr;
	const float MaxWalkSpeed = MovementComponent ? MovementComponent->MaxWalkSpeed : 0.f;
	if (PawnOwner && MaxWalkSpeed > UE_KINDA_SMALL_NUMBER)
	{
		const FRotator YawRotation(0.0, PawnOwner->GetActorRotation().Yaw, 0.0);
		const FVector Forward = YawRotation.Vector();
		const FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		FVector2D Move(
			FVector::DotProduct(GroundVelocity, Forward) / MaxWalkSpeed,
			FVector::DotProduct(GroundVelocity, Right) / MaxWalkSpeed);
		if (Move.SizeSquared() > 1.0)
		{
			Move.Normalize();
		}
		MoveX = static_cast<float>(Move.X);
		MoveY = static_cast<float>(Move.Y);
	}
	else
	{
		MoveX = 0.f;
		MoveY = 0.f;
	}

	// task-386-21: debug override of the move vector (rr.Debug.MinerAnimForce), so each blend space sample can be captured.
	FVector2D ForcedMove;
	if (RRMinerAnimDebug::TryGetForcedMove(ForcedMove))
	{
		MoveX = static_cast<float>(ForcedMove.X);
		MoveY = static_cast<float>(ForcedMove.Y);
		GroundSpeed = static_cast<float>(ForcedMove.Size()) * MaxWalkSpeed;
	}

	ApplyDebugUpperPose();
	ApplyDebugAnimFreeze();
	ApplyDebugRightHandOffset();

	LogDebugValues();
}

void URRMinerAnimInstance::ApplyDebugUpperPose()
{
	// task-408-01: only the player miner follows the cvar; the asset is resolved once per new value, not every frame.
	const APawn* PawnOwner = TryGetPawnOwner();
	if (!PawnOwner || !PawnOwner->IsPlayerControlled())
	{
		return;
	}

	const FString PoseName = RRMinerAnimDebug::CVarMinerUpperPose.GetValueOnGameThread().TrimStartAndEnd();
	if (PoseName == AppliedDebugUpperPoseName)
	{
		return;
	}
	AppliedDebugUpperPoseName = PoseName;

	if (PoseName.IsEmpty())
	{
		// The class default of the ABP generated class holds the UpperBodySequence set in the ABP_Miner class defaults.
		UpperBodySequence = GetClass()->GetDefaultObject<URRMinerAnimInstance>()->UpperBodySequence;
		UE_LOG(LogRogueRigs, Log, TEXT("MinerUpperPose %s: class default %s"), *GetNameSafe(PawnOwner), *GetNameSafe(UpperBodySequence));
		return;
	}

	const FString AssetPath = FString::Printf(TEXT("%s%s.%s"), RRMinerAnimDebug::UpperPoseFolder, *PoseName, *PoseName);
	UAnimSequenceBase* Sequence = LoadObject<UAnimSequenceBase>(nullptr, *AssetPath, nullptr, LOAD_NoWarn | LOAD_Quiet);
	if (!Sequence)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("rr.Debug.MinerUpperPose: no anim sequence at %s; UpperBodySequence kept (%s)"),
			*AssetPath, *GetNameSafe(UpperBodySequence));
		return;
	}
	UpperBodySequence = Sequence;
	UE_LOG(LogRogueRigs, Log, TEXT("MinerUpperPose %s: %s"), *GetNameSafe(PawnOwner), *AssetPath);
}

void URRMinerAnimInstance::ApplyDebugAnimFreeze()
{
	USkeletalMeshComponent* MinerMesh = GetSkelMeshComponent();
	if (!MinerMesh)
	{
		return;
	}

	// The scale applies from the next tick (this update already received its delta time); update and evaluation keep
	// running with a zero delta, so a pose source change (rr.Debug.MinerUpperPose) still shows.
	const bool bWantFrozen = RRMinerAnimDebug::CVarMinerAnimFreeze.GetValueOnGameThread() != 0;
	if (bWantFrozen && !bDebugAnimFrozen)
	{
		SavedGlobalAnimRateScale = MinerMesh->GlobalAnimRateScale;
		MinerMesh->GlobalAnimRateScale = 0.f;
		bDebugAnimFrozen = true;
	}
	else if (!bWantFrozen && bDebugAnimFrozen)
	{
		MinerMesh->GlobalAnimRateScale = SavedGlobalAnimRateScale;
		bDebugAnimFrozen = false;
	}
}

void URRMinerAnimInstance::ApplyDebugRightHandOffset()
{
	// task-410-01: same pattern as rr.Debug.MinerUpperPose: player miner only, work only when the cvar value changes.
	const APawn* PawnOwner = TryGetPawnOwner();
	if (!PawnOwner || !PawnOwner->IsPlayerControlled())
	{
		return;
	}

	const FString Value = RRMinerAnimDebug::CVarRightHandOffset.GetValueOnGameThread().TrimStartAndEnd();
	if (Value == AppliedDebugRightHandOffset)
	{
		return;
	}
	AppliedDebugRightHandOffset = Value;

	if (Value.IsEmpty())
	{
		// The class default of the ABP generated class holds the values set in the ABP_Miner class defaults.
		const URRMinerAnimInstance* ClassDefaults = GetClass()->GetDefaultObject<URRMinerAnimInstance>();
		RightHandOffsetLocation = ClassDefaults->RightHandOffsetLocation;
		RightHandOffsetRotation = ClassDefaults->RightHandOffsetRotation;
		RefreshRightHandOffset();
		return;
	}

	FVector Location;
	FRotator Rotation;
	if (!RRMinerAnimDebug::TryParseRightHandOffset(Value, Location, Rotation))
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("rr.Debug.RightHandOffset: bad value \"%s\" (expected \"X Y Z\" or \"X Y Z Pitch Yaw Roll\", floats); offset kept"), *Value);
		return;
	}
	RightHandOffsetLocation = Location;
	RightHandOffsetRotation = Rotation;
	RefreshRightHandOffset();
}

void URRMinerAnimInstance::RefreshRightHandOffset()
{
	const USkeletalMeshComponent* MinerMesh = GetSkelMeshComponent();
	if (!MinerMesh)
	{
		return;
	}

	// Socket the offset is expressed in: the hand socket of the held weapon, else WeaponSocket_R.
	FName SocketName = DefaultRightHandSocketName;
	if (const ARRMiner* Miner = Cast<ARRMiner>(TryGetPawnOwner()))
	{
		for (const ARRWeapon* Weapon : Miner->GetWeapons())
		{
			if (Weapon && !Weapon->GetHandSocketName().IsNone())
			{
				SocketName = Weapon->GetHandSocketName();
				break;
			}
		}
	}

	const USkeletalMeshSocket* HandSocket = MinerMesh->GetSocketByName(SocketName);
	if (!HandSocket)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("RightHandOffset %s: no socket %s on the miner mesh; right hand offset off"),
			*GetNameSafe(TryGetPawnOwner()), *SocketName.ToString());
		RightHandIKEffector = FVector::ZeroVector;
		RightHandRotationDelta = FRotator::ZeroRotator;
		RightHandOffsetAlpha = 0.f;
		AppliedHandOffset = FTransform::Identity;
		// The hand does not move, so the hand-held weapon gets its Blueprint values back.
		if (const ARRMiner* Miner = Cast<ARRMiner>(TryGetPawnOwner()))
		{
			for (const ARRWeapon* Weapon : Miner->GetWeapons())
			{
				RefreshHeldWeapon(Weapon);
			}
		}
		return;
	}

	const bool bHasOffset = !RightHandOffsetLocation.IsNearlyZero() || !RightHandOffsetRotation.IsNearlyZero();
	if (bHasOffset)
	{
		// Socket frame -> RightHand bone space, with the pivot at the socket point. UE order: A * B applies A, then B.
		// S = socket local transform (socket relative to its bone, RightHand); its scale (about 0.01) cancels the RightHand
		// bone scale (about 100), so the socket frame is in cm. O = offset in the socket frame (p -> R * p + L: turn around
		// the socket origin, the grip, then move by L cm). H = RightHand in component space; socket = S * H.
		// The socket must end at O * S * H while the hand stays rigid on it, so the new hand is H' = D * H with
		// D = S^-1 * O * S, a transform in RightHand bone space. Conjugating O by S is the pivot correction: D turns the
		// hand around the socket point (D maps the socket point S(0) to S(L)), not around the hand bone origin.
		// Translation of D = where the RightHand bone must go, in its own bone space (bone units, scaled by the bone scale,
		// like LeftHandIKTarget): the Two Bone IK effector; the IK keeps the hand component-space rotation, giving
		// FTransform(t) * H. Rotation of D = the bone-space rotation added after the IK by Transform (Modify) Bone
		// (Add to Existing, Bone Space): FTransform(Q) * FTransform(t) * H = D * H. The scales of S and S^-1 cancel in D.
		const FTransform SocketLocal = HandSocket->GetSocketLocalTransform();
		const FTransform Offset(RightHandOffsetRotation, RightHandOffsetLocation);
		const FTransform HandDelta = SocketLocal.Inverse() * Offset * SocketLocal;
		RightHandIKEffector = HandDelta.GetLocation();
		RightHandRotationDelta = HandDelta.Rotator();
		RightHandOffsetAlpha = 1.f;
		AppliedHandOffset = Offset;
	}
	else
	{
		// Offset 0 = the upper body pose unchanged: right arm IK and hand rotation off.
		RightHandIKEffector = FVector::ZeroVector;
		RightHandRotationDelta = FRotator::ZeroRotator;
		RightHandOffsetAlpha = 0.f;
		AppliedHandOffset = FTransform::Identity;
	}

	// task-411-01: the hand now moves by the new offset; counter-move the hand-held weapon (the rifle keeps its place),
	// then recompute the left hand target from the grip.
	FString HeldWeaponSummary = TEXT("no hand-held weapon");
	if (const ARRMiner* Miner = Cast<ARRMiner>(TryGetPawnOwner()))
	{
		for (const ARRWeapon* Weapon : Miner->GetWeapons())
		{
			RefreshHeldWeapon(Weapon, &HeldWeaponSummary);
		}
	}

	// One line per apply; the two first values paste into the ABP_Miner class defaults (Animation|RightHand).
	UE_LOG(LogRogueRigs, Log,
		TEXT("RightHandOffset %s: RightHandOffsetLocation=(X=%.3f,Y=%.3f,Z=%.3f) RightHandOffsetRotation=(Pitch=%.3f,Yaw=%.3f,Roll=%.3f) | socket %s on bone %s -> RightHandIKEffector=(X=%.5f,Y=%.5f,Z=%.5f) RightHandRotationDelta=(Pitch=%.3f,Yaw=%.3f,Roll=%.3f) RightHandOffsetAlpha=%.0f | counter-move: %s"),
		*GetNameSafe(TryGetPawnOwner()),
		RightHandOffsetLocation.X, RightHandOffsetLocation.Y, RightHandOffsetLocation.Z,
		RightHandOffsetRotation.Pitch, RightHandOffsetRotation.Yaw, RightHandOffsetRotation.Roll,
		*SocketName.ToString(), *HandSocket->BoneName.ToString(),
		RightHandIKEffector.X, RightHandIKEffector.Y, RightHandIKEffector.Z,
		RightHandRotationDelta.Pitch, RightHandRotationDelta.Yaw, RightHandRotationDelta.Roll,
		RightHandOffsetAlpha, *HeldWeaponSummary);
}

bool URRMinerAnimInstance::IsHeldOnHandSocket(const ARRWeapon* Weapon) const
{
	const USkeletalMeshComponent* MinerMesh = GetSkelMeshComponent();
	const FName HandSocketName = Weapon ? Weapon->GetHandSocketName() : NAME_None;
	return Weapon && MinerMesh && !HandSocketName.IsNone()
		&& Weapon->GetAttachParentActor() == MinerMesh->GetOwner()
		&& Weapon->GetAttachParentSocketName() == HandSocketName;
}

void URRMinerAnimInstance::RefreshHeldWeapon(const ARRWeapon* Weapon, FString* OutSummary)
{
	if (!IsHeldOnHandSocket(Weapon))
	{
		return;
	}
	// Counter-move first: RefreshLeftHandIK reads the grip from the weapon components, which must already be in place.
	ApplyRifleCounterMove(Weapon, OutSummary);
	RefreshLeftHandIK(Weapon);
}

bool URRMinerAnimInstance::ApplyRifleCounterMove(const ARRWeapon* Weapon, FString* OutSummary)
{
	if (!IsHeldOnHandSocket(Weapon))
	{
		return false;
	}
	const USceneComponent* WeaponRoot = Weapon->GetRootComponent();
	if (!WeaponRoot)
	{
		return false;
	}

	// Why R' = R * O^-1 keeps the rifle world transform (UE order: A * B applies A, then B).
	// Socket world before the offset: Sock = S * H. RefreshRightHandOffset moves the hand to H' = D * H with D = S^-1 * O * S,
	// so the socket goes to S * D * H = O * Sock. The weapon root rides on the socket: W = C * Sock (C = root relative to the
	// socket, identity for the snapped rifle) becomes C * O * Sock. A component under the root with relative transform R is
	// at R * W; to stay at R * C * Sock it needs R' with R' * C * O * Sock = R * C * Sock, i.e. R' = R * (C * O^-1 * C^-1),
	// which is R' = R * O^-1 when C is identity. The counter-move is a fixed relative transform under the hand, so a hand
	// animation still carries the rifle. R is always the Blueprint value, so repeated applies never accumulate; O identity
	// (offset 0) gives R back exactly.
	const FTransform RootToSocket = WeaponRoot->GetRelativeTransform();
	const FTransform CounterMove = RootToSocket * AppliedHandOffset.Inverse() * RootToSocket.Inverse();
	const bool bIdentity = AppliedHandOffset.Equals(FTransform::Identity, 0.0);

	FString MeshText = TEXT("none");
	TArray<UMeshComponent*> MeshComponents;
	Weapon->GetComponents<UMeshComponent>(MeshComponents);
	for (UMeshComponent* WeaponMesh : MeshComponents)
	{
		if (!WeaponMesh->DoesSocketExist(LeftHandGripSocketName))
		{
			continue;
		}
		if (WeaponMesh->GetAttachParent() != WeaponRoot)
		{
			UE_LOG(LogRogueRigs, Warning, TEXT("RightHandOffset: %s %s is not attached to the weapon root; not counter-moved"),
				*GetNameSafe(Weapon), *WeaponMesh->GetName());
			continue;
		}
		const FTransform BlueprintRelative = GetBlueprintRelativeTransform(WeaponMesh);
		WeaponMesh->SetRelativeTransform(bIdentity ? BlueprintRelative : BlueprintRelative * CounterMove);
		MeshText = FString::Printf(TEXT("%s relative=%s"), *WeaponMesh->GetName(),
			*RRMinerAnimDebug::DescribeRelativeTransform(WeaponMesh->GetRelativeTransform()));
		break;
	}

	FString MuzzleText = TEXT("none");
	USceneComponent* Muzzle = Weapon->GetMuzzle();
	if (Muzzle && Muzzle->GetAttachParent() == WeaponRoot)
	{
		const FTransform BlueprintRelative = GetBlueprintRelativeTransform(Muzzle);
		Muzzle->SetRelativeTransform(bIdentity ? BlueprintRelative : BlueprintRelative * CounterMove);
		MuzzleText = FString::Printf(TEXT("%s relative=%s"), *Muzzle->GetName(),
			*RRMinerAnimDebug::DescribeRelativeTransform(Muzzle->GetRelativeTransform()));
	}
	else if (Muzzle)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("RightHandOffset: %s Muzzle is not attached to the weapon root; not counter-moved"), *GetNameSafe(Weapon));
	}

	if (OutSummary)
	{
		*OutSummary = FString::Printf(TEXT("%s RifleMesh %s Muzzle %s"), *GetNameSafe(Weapon), *MeshText, *MuzzleText);
	}
	return true;
}

FTransform URRMinerAnimInstance::GetBlueprintRelativeTransform(USceneComponent* Component)
{
	// Only this anim instance moves these components, so the value seen before its first move is the Blueprint value.
	if (const FTransform* Saved = BlueprintRelativeTransforms.Find(Component))
	{
		return *Saved;
	}
	return BlueprintRelativeTransforms.Add(Component, Component->GetRelativeTransform());
}

void URRMinerAnimInstance::RestoreCounterMovedComponents()
{
	// A later anim instance on the same weapon must save the true Blueprint values, not a counter-moved state.
	for (const TPair<TWeakObjectPtr<USceneComponent>, FTransform>& Entry : BlueprintRelativeTransforms)
	{
		if (USceneComponent* Component = Entry.Key.Get())
		{
			Component->SetRelativeTransform(Entry.Value);
		}
	}
	BlueprintRelativeTransforms.Reset();
}

const FName URRMinerAnimInstance::LeftHandGripSocketName(TEXT("LeftHandGrip"));

const FName URRMinerAnimInstance::DefaultRightHandSocketName(TEXT("WeaponSocket_R"));

void URRMinerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	// task-410-01: right hand offset from the class defaults (the cvar, if set, is applied on the first update).
	// task-411-01: this also counter-moves the weapons already held, then refreshes their left hand IK (RefreshHeldWeapon),
	// so the weapons acquired before this anim instance started (the delegate would not fire for them) are covered.
	RefreshRightHandOffset();

	ARRMiner* Miner = Cast<ARRMiner>(TryGetPawnOwner());
	if (!Miner)
	{
		return;
	}
	BoundMiner = Miner;
	Miner->OnWeaponAdded.AddUniqueDynamic(this, &URRMinerAnimInstance::HandleWeaponAdded);
}

void URRMinerAnimInstance::NativeUninitializeAnimation()
{
	if (ARRMiner* Miner = BoundMiner.Get())
	{
		Miner->OnWeaponAdded.RemoveDynamic(this, &URRMinerAnimInstance::HandleWeaponAdded);
	}
	BoundMiner.Reset();
	RestoreCounterMovedComponents();
	Super::NativeUninitializeAnimation();
}

void URRMinerAnimInstance::HandleWeaponAdded(ARRWeapon* Weapon)
{
	// task-411-01: counter-move by the current offset first, then the left hand IK from the moved grip.
	RefreshHeldWeapon(Weapon);
}

void URRMinerAnimInstance::RefreshLeftHandIK(const ARRWeapon* Weapon)
{
	// Only a hand-held weapon (attached to its hand socket on the miner mesh) drives the left hand.
	const USkeletalMeshComponent* MinerMesh = GetSkelMeshComponent();
	if (!IsHeldOnHandSocket(Weapon))
	{
		return;
	}
	const FName HandSocketName = Weapon->GetHandSocketName();

	// Result for this hand-held weapon: off until a mesh with LeftHandGrip is found.
	LeftHandIKAlpha = 0.f;
	LeftHandIKTarget = FVector::ZeroVector;

	const USkeletalMeshSocket* HandSocket = MinerMesh->GetSocketByName(HandSocketName);
	if (!HandSocket)
	{
		return;
	}

	TArray<UMeshComponent*> MeshComponents;
	Weapon->GetComponents<UMeshComponent>(MeshComponents);
	for (const UMeshComponent* WeaponMesh : MeshComponents)
	{
		if (!WeaponMesh->DoesSocketExist(LeftHandGripSocketName))
		{
			continue;
		}
		// The weapon is rigid on WeaponSocket_R, so the grip in the weapon actor space moved by the socket local transform
		// (bone -> socket) gives the grip in the bone space of the right hand, whatever the current pose. The grip is read
		// after the task-411-01 counter-move, so with a right hand offset the target is the grip seen from the moved hand.
		const FTransform GripWorld = WeaponMesh->GetSocketTransform(LeftHandGripSocketName, RTS_World);
		const FVector GripInActor = GripWorld.GetRelativeTransform(Weapon->GetActorTransform()).GetLocation();
		LeftHandIKTarget = HandSocket->GetSocketLocalTransform().TransformPosition(GripInActor);
		LeftHandIKAlpha = 1.f;
		return;
	}
}

void URRMinerAnimInstance::LogDebugValues()
{
	if (RRMinerAnimDebug::CVarMinerAnim.GetValueOnGameThread() == 0)
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const double Now = World->GetTimeSeconds();
	if (LastDebugLogTime >= 0.0 && Now - LastDebugLogTime < RRMinerAnimDebug::LogIntervalSeconds)
	{
		return;
	}
	LastDebugLogTime = Now;

	UE_LOG(LogRogueRigs, Log, TEXT("MinerAnim %s: MoveX=%.3f MoveY=%.3f GroundSpeed=%.1f"),
		*GetNameSafe(TryGetPawnOwner()), MoveX, MoveY, GroundSpeed);
}

float URRMinerAnimInstance::ComputeRunSpeedForDirection(float InDirection) const
{
	const float AbsDirection = FMath::Abs(InDirection);
	const float SideSpeed = InDirection >= 0.f ? RunSpeedRight : RunSpeedLeft;

	float Speed;
	if (AbsDirection <= 90.f)
	{
		// Forward to side.
		Speed = FMath::Lerp(RunSpeedForward, SideSpeed, AbsDirection / 90.f);
	}
	else
	{
		// Side to backward.
		Speed = FMath::Lerp(SideSpeed, RunSpeedBackward, FMath::Min(AbsDirection - 90.f, 90.f) / 90.f);
	}

	// Guard the division in NativeUpdateAnimation against a zero speed set by hand.
	return FMath::Max(Speed, 1.f);
}
