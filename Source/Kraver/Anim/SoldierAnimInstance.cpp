// Fill out your copyright notice in the Description page of Project Settings.


#include "SoldierAnimInstance.h"

USoldierAnimInstance::USoldierAnimInstance() : UCreatureAnimInstance()
{
}

void USoldierAnimInstance::NativeUpdateAnimation(float DeltaSeconds) 
{
	UCreatureAnimInstance::NativeUpdateAnimation(DeltaSeconds);

	Soldier = Cast<ASoldier>(Creature);
	if(Soldier == nullptr)
		return;

	IsEquippingWeapon = Soldier->CombatComponent->GetCurWeapon() != nullptr;
	if (Soldier->CombatComponent->GetCurWeapon() && Soldier->GetMesh())
	{
		LeftHandTransform = Soldier->CombatComponent->GetCurWeapon()->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		Soldier->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));
	}
}
