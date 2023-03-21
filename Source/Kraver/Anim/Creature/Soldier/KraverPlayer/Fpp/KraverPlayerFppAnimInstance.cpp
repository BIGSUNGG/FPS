// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverPlayerFppAnimInstance.h"
#include "Kraver/Creature/Soldier/KraverPlayer/KraverPlayer.h"

void UKraverPlayerFppAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	UKraverPlayerAnimInstance::NativeUpdateAnimation(DeltaSeconds);

	if(KraverPlayer == nullptr) 
		return;

	if(!KraverPlayer->IsLocallyControlled())
		return;

	if (IsSubAttacking)
	{
		RightHandFppRotation = KraverPlayer->GetRightHandFppRotation();
	}
	else
	{
	
	}

	if (KraverPlayer->CombatComponent->GetCurWeapon() && KraverPlayer->GetArmWeaponMeshes()[KraverPlayer->CombatComponent->GetCurWeapon()] && KraverPlayer->GetArmMesh())
	{
		FabrikLeftHand(KraverPlayer->GetArmMesh(), KraverPlayer->GetArmWeaponMeshes()[KraverPlayer->CombatComponent->GetCurWeapon()], LeftHandFppFabrikTransform);
	}
}

void UKraverPlayerFppAnimInstance::AnimNotify_RefillAmmo()
{
	if (KraverPlayer->CombatComponent->GetCurWeapon() == nullptr)
		return;

	KraverPlayer->CombatComponent->GetCurWeapon()->RefillAmmo();
}