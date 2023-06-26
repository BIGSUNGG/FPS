// Fill out your copyright notice in the Description page of Project Settings.


#include "Soldier.h"
#include "Kraver/Animation/Creature/Soldier/SoldierAnimInstance.h"
#include "Kraver/Component/Attachment/Weapon/Magazine/AttachmentMagazineComponent.h"

ASoldier::ASoldier()
	: ACreature()
{

}

void ASoldier::BeginPlay()
{
	ACreature::BeginPlay();

	USoldierAnimInstance* AnimInstance = Cast<USoldierAnimInstance>(GetMesh()->GetAnimInstance());
	AnimInstance->OnReload_Grab_Magazine.AddDynamic(this, &ASoldier::OnReload_Grab_MagazineEvent);
	AnimInstance->OnReload_Insert_Magazine.AddDynamic(this, &ASoldier::OnReload_Insert_MagazineEvent);
}

void ASoldier::Tick(float DeltaTime)
{
	ACreature::Tick(DeltaTime);
}

void ASoldier::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	ACreature::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASoldier::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	ACreature::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void ASoldier::OnReload_Grab_MagazineEvent()
{
	if (CombatComponent->GetCurWeapon() && CombatComponent->GetCurWeapon()->GetWeaponPrimitiveInfo().Contains("Magazine"))
	{
		CombatComponent->GetCurWeapon()->GetWeaponPrimitiveInfo()["Magazine"]->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "SOCKET_Magazine");
	}
}

void ASoldier::OnReload_Insert_MagazineEvent()
{
	if (CombatComponent->GetCurWeapon() && CombatComponent->GetCurWeapon()->GetWeaponPrimitiveInfo().Contains("Magazine"))
	{
		UAttachmentMagazineComponent* MagazineComp = CombatComponent->GetCurWeapon()->FindComponentByClass<UAttachmentMagazineComponent>();
		CombatComponent->GetCurWeapon()->GetWeaponPrimitiveInfo()["Magazine"]->AttachToComponent(CombatComponent->GetCurWeapon()->GetWeaponMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, MagazineComp->GetMagazineSocketName());
	}
}
