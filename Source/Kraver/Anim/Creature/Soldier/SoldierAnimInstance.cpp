// Fill out your copyright notice in the Description page of Project Settings.


#include "SoldierAnimInstance.h"
#include "Kraver/Creature/Soldier/Soldier.h"

USoldierAnimInstance::USoldierAnimInstance() : UCreatureAnimInstance()
{
}

void USoldierAnimInstance::NativeBeginPlay()
{
	UCreatureAnimInstance::NativeBeginPlay();

	Soldier = Cast<ASoldier>(Creature);
}

void USoldierAnimInstance::NativeUpdateAnimation(float DeltaSeconds) 
{
	UCreatureAnimInstance::NativeUpdateAnimation(DeltaSeconds);

	if (Soldier == nullptr)
		return;

	IsEquippingWeapon = Soldier->CombatComponent->GetCurWeapon() != nullptr;
	if (Soldier->CombatComponent->GetCurWeapon() && Soldier->GetMesh())
	{
		FabrikLeftHand(Soldier->GetMesh(), Soldier->CombatComponent->GetCurWeapon()->GetWeaponMesh(), LeftHandFabrikTransform);
	}	
}

void USoldierAnimInstance::FabrikLeftHand(USkeletalMeshComponent* HandMesh, USkeletalMeshComponent* WeaponMesh, FTransform& Transform)
{
	Transform = WeaponMesh->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
	FVector OutPosition;
	FRotator OutRotation;
	HandMesh->TransformToBoneSpace("hand_r", Transform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
	Transform.SetLocation(OutPosition);
	Transform.SetRotation(FQuat(OutRotation));

}
