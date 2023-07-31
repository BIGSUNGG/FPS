// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverPlayerFppAnimInstance.h"
#include "Kraver/Character/Creature/Soldier/KraverPlayer/KraverPlayer.h"

void UKraverPlayerFppAnimInstance::NativeBeginPlay()
{
	UKraverPlayerAnimInstance::NativeBeginPlay();

	KraverPlayer->OnLand.AddDynamic(this, &ThisClass::OnLandEvent);
	KraverPlayer->OnJump.AddDynamic(this, &ThisClass::OnJumpEvent);
	KraverPlayer->OnCrouchStart.AddDynamic(this, &ThisClass::OnStartCrouchEvent);
	KraverPlayer->OnCrouchEnd.AddDynamic(this, &ThisClass::OnEndCrouchEvent);
}

void UKraverPlayerFppAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	UKraverPlayerAnimInstance::NativeUpdateAnimation(DeltaSeconds);

	if(KraverPlayer == nullptr) 
		return;

	if (!KraverPlayer->IsLocallyControlled())
		return;

	for (auto& State : ProceduralAnimStates)
	{
		State.Key.TickTimeline(DeltaSeconds);
	}

	USkeletalMeshComponent* Fp_WeaponMesh = dynamic_cast<USkeletalMeshComponent*>(KraverPlayer->CombatComponent->GetCurWeapon() ? KraverPlayer->GetArmWeaponMeshes()[KraverPlayer->CombatComponent->GetCurWeapon()]->operator[]("Root") : nullptr);

	if (IsEquippingWeapon)
	{
		AnimWeaponIdleFpp = CurWeapon->GetAnimIdleFpp();
		AnimWeaponSubAttackFpp = CurWeapon->GetSubAttackFpp();
		AnimWeaponMovementFpp = CurWeapon->GetAnimMovementFpp();
	}
	else
	{
		AnimWeaponIdleFpp = nullptr;
		AnimWeaponSubAttackFpp = nullptr;
		AnimWeaponMovementFpp = nullptr;
	}

	if (KraverPlayer->CombatComponent->GetCurWeapon() && KraverPlayer->GetArmWeaponMeshes()[KraverPlayer->CombatComponent->GetCurWeapon()] && KraverPlayer->GetArmMesh())
	{
		FabrikLeftHand(KraverPlayer->GetArmMesh(), Fp_WeaponMesh, LeftHandFppFabrikTransform);
	}

	ProceduralAnimEvent();
	WeaponSway(DeltaSeconds);

}

void UKraverPlayerFppAnimInstance::WeaponSway(float DeltaSeconds)
{
#if TEST_ADS
	return;
#endif

	float TurnValue = KraverPlayer->GetInputAxisValue("Turn") * SwayValue;
	float LookUpValue = KraverPlayer->GetInputAxisValue("LookUp") * SwayValue;
	FRotator WeaponSwayFinalRot;
	FRotator WeaponSwayInitRot;

	WeaponSwayFinalRot.Roll = LookUpValue * SwayValue;
	WeaponSwayFinalRot.Yaw = TurnValue * SwayValue;
	WeaponSwayFinalRot.Pitch = TurnValue * SwayValue;

	FRotator TargetRot;
	TargetRot.Roll = WeaponSwayInitRot.Roll - WeaponSwayFinalRot.Roll;
	TargetRot.Yaw = WeaponSwayInitRot.Yaw + WeaponSwayFinalRot.Yaw;
	TargetRot.Pitch = WeaponSwayInitRot.Pitch + WeaponSwayFinalRot.Pitch;

	WeaponSwayResultRot = FMath::RInterpTo(WeaponSwayResultRot, TargetRot, DeltaSeconds, 4.f);

	if (WeaponSwayResultRot.Roll > MaxSwayDegree)
		WeaponSwayResultRot.Roll = MaxSwayDegree;
	else if (WeaponSwayResultRot.Roll < MinSwayDegree)
		WeaponSwayResultRot.Roll = MinSwayDegree;

	if (WeaponSwayResultRot.Yaw > MaxSwayDegree)
		WeaponSwayResultRot.Yaw = MaxSwayDegree;
	else if (WeaponSwayResultRot.Yaw < MinSwayDegree)
		WeaponSwayResultRot.Yaw = MinSwayDegree;

	if (WeaponSwayResultRot.Roll > MaxSwayDegree)
		WeaponSwayResultRot.Roll = MaxSwayDegree;
	else if (WeaponSwayResultRot.Roll < MinSwayDegree)
		WeaponSwayResultRot.Roll = MinSwayDegree;

	if (WeaponSwayResultRot.Pitch > MaxSwayDegree)
		WeaponSwayResultRot.Pitch = MaxSwayDegree;
	else if (WeaponSwayResultRot.Pitch < MinSwayDegree)
		WeaponSwayResultRot.Pitch = MinSwayDegree;

	if (KraverPlayer->CombatComponent->GetCurWeapon() && KraverPlayer->CombatComponent->GetCurWeapon()->GetIsSubAttacking())
		WeaponSwayResultRot *= 0.9f;

}


void UKraverPlayerFppAnimInstance::StartProceduralAnim(float Strength)
{
	for (int i = 0; i < ProceduralAnimStates.Num(); i++)
	{
		float TimelineValue = ProceduralAnimStates[i].Key.GetPlaybackPosition();

		if (TimelineValue >= ProceduralAnimStates[i].Key.GetTimelineLength())
			ProceduralAnimStates.RemoveAt(i);
	}

	int Index = ProceduralAnimStates.Add(TTuple<FTimeline, float>());

	ProceduralAnimStates[Index].Value = Strength;

	FOnTimelineFloat TimelineCallback;

	ProceduralAnimStates[Index].Key.AddInterpFloat(ProceduralAnimCurve, TimelineCallback);
	ProceduralAnimStates[Index].Key.PlayFromStart();

}

void UKraverPlayerFppAnimInstance::ProceduralAnimEvent()
{
	ProceduralAnimResultVec = FVector::ZeroVector;

	for(int i = 0; i < ProceduralAnimStates.Num(); i++)
	{
		float TimelineValue = ProceduralAnimStates[i].Key.GetPlaybackPosition();
		float CurveFloatValue = ProceduralAnimCurve->GetFloatValue(TimelineValue);

		ProceduralAnimResultVec.Z += UKismetMathLibrary::Lerp(0.f, -10.f, CurveFloatValue * ProceduralAnimStates[i].Value);

	}
	if (KraverPlayer->CombatComponent->GetCurWeapon() && KraverPlayer->CombatComponent->GetCurWeapon()->GetIsSubAttacking())
		ProceduralAnimResultVec.Z *= 0.1f;
}

void UKraverPlayerFppAnimInstance::OnLandEvent(const FHitResult& Result)
{
	StartProceduralAnim(0.35f);
}

void UKraverPlayerFppAnimInstance::OnJumpEvent()
{
	StartProceduralAnim(-0.4f);
}

void UKraverPlayerFppAnimInstance::OnStartCrouchEvent(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	StartProceduralAnim(-0.25f);
}

void UKraverPlayerFppAnimInstance::OnEndCrouchEvent(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	StartProceduralAnim(0.25f);
}
