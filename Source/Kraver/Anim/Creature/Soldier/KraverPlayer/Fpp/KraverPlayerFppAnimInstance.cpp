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
	
	USkeletalMeshComponent* ArmWeaponMesh = nullptr;
	if (KraverPlayer->CombatComponent->GetCurWeapon())
		ArmWeaponMesh = KraverPlayer->GetArmWeaponMeshes()[KraverPlayer->CombatComponent->GetCurWeapon()];	

	if (IsSubAttacking)
	{
		FRotator AimSocketRotation = ArmWeaponMesh->GetSocketRotation("SOCKET_Aim");
		FRotator HandSocketRotation = KraverPlayer->GetArmMesh()->GetSocketRotation(KraverPlayer->CombatComponent->GetCurWeapon()->GetAttachSocketName());
		FRotator DifferenceRotation = HandSocketRotation - AimSocketRotation;

		RightHandFppRotation.Pitch = -KraverPlayer->GetCamera()->GetComponentRotation().Pitch;
		RightHandFppRotation.Roll = -KraverPlayer->GetCamera()->GetComponentRotation().Roll;
		RightHandFppRotation.Yaw = KraverPlayer->GetCamera()->GetComponentRotation().Yaw - DifferenceRotation.Yaw;
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