// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverPlayerFppAnimInstance.h"
#include "Kraver/Creature/Soldier/KraverPlayer/KraverPlayer.h"

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

	ProceduralAnimTimeLine.TickTimeline(DeltaSeconds);
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

}

void UKraverPlayerFppAnimInstance::StartProceduralAnim(float Strength)
{
	ProceduralAnimStrength = Strength;

	FOnTimelineFloat TimelineCallback;
	TimelineCallback.BindUFunction(this, FName("ProceduralAnimEvent"));

	ProceduralAnimTimeLine.AddInterpFloat(ProceduralAnimCurve, TimelineCallback);
	ProceduralAnimTimeLine.PlayFromStart();
}

void UKraverPlayerFppAnimInstance::ProceduralAnimEvent()
{
	float TimelineValue = ProceduralAnimTimeLine.GetPlaybackPosition();
	float CurveFloatValue = ProceduralAnimCurve->GetFloatValue(TimelineValue);

	ProceduralAnimResultVec.Z = UKismetMathLibrary::Lerp(0.f, -10.f, CurveFloatValue * ProceduralAnimStrength);

	if (KraverPlayer->CombatComponent->GetCurWeapon() && KraverPlayer->CombatComponent->GetCurWeapon()->GetIsSubAttacking())
		ProceduralAnimResultVec.Z *= 0.1f;
}

void UKraverPlayerFppAnimInstance::OnLandEvent(const FHitResult& Result)
{
	StartProceduralAnim(0.75f);
}

void UKraverPlayerFppAnimInstance::OnJumpEvent()
{
	StartProceduralAnim(-0.75f);
}

void UKraverPlayerFppAnimInstance::OnStartCrouchEvent(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	StartProceduralAnim(-0.5f);
}

void UKraverPlayerFppAnimInstance::OnEndCrouchEvent(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	StartProceduralAnim(0.5f);
}
