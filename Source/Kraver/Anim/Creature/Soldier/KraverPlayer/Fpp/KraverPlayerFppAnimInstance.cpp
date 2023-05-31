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
		FabrikLeftHand(KraverPlayer->GetArmMesh(), KraverPlayer->GetArmWeaponMeshes()[KraverPlayer->CombatComponent->GetCurWeapon()], LeftHandFppFabrikTransform);
	}

	ProceduralAnimResultVec = KraverPlayer->GetProceduralAnimResultVec();
}
