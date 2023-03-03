// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverPlayerFppAnimInstance.h"
#include "Kraver/Creature/Soldier/KraverPlayer/KraverPlayer.h"

void UKraverPlayerFppAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	UKraverPlayerAnimInstance::NativeUpdateAnimation(DeltaSeconds);

	if (KraverPlayer->CombatComponent->GetCurWeapon() && KraverPlayer->GetArmWeaponMesh() && KraverPlayer->GetArmMesh())
	{
		FabrikLeftHand(KraverPlayer->GetArmMesh(), KraverPlayer->GetArmWeaponMesh(), LeftHandFppFabrikTransform);
	}
}

void UKraverPlayerFppAnimInstance::AnimNotify_RefillAmmo()
{
	if (KraverPlayer->CombatComponent->GetCurWeapon() == nullptr)
		return;

	KraverPlayer->CombatComponent->GetCurWeapon()->RefillAmmo();
	KR_LOG(Log, TEXT("Refill"));
}