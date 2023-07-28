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

	WeaponSwayResultRot = KraverPlayer->GetWeaponSwayResultRot();
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
