// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverPlayerFppAnimInstance.h"
#include "Kraver/Creature/Soldier/KraverPlayer/KraverPlayer.h"
#include "Kraver/KraverComponent/ProceduralAnimation/PlayerProceduralAnimComponent.h"

void UKraverPlayerFppAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	UKraverPlayerAnimInstance::NativeUpdateAnimation(DeltaSeconds);

	if(KraverPlayer == nullptr) 
		return;

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

	ProceduralAnimResultVec = KraverPlayer->PlayerProceduralAnimComponent->GetProceduralAnimResultVec();
}
