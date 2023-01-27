// Fill out your copyright notice in the Description page of Project Settings.


#include "Soldier.h"
#include "Net/UnrealNetwork.h"

ASoldier::ASoldier()
	: ACreature()
{
}

void ASoldier::BeginPlay()
{
	ACreature::BeginPlay();

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
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASoldier, CurWeapon);
}

void ASoldier::Server_EquipWeapon_Implementation(AWeapon* Weapon)
{
	CurWeapon = Weapon;
	CurWeapon->SetOwner(this);

	CurWeapon->GetWeaponMesh()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, CurWeapon->GetAttachSocketName());
}

void ASoldier::EquipWeapon(AWeapon* Weapon)
{
	if(!Weapon)
	 return;

	CurWeapon = Weapon;
	Server_EquipWeapon(Weapon);

	CurWeapon->SetOwner(this);
	CurWeapon->Equipped(this);

	CurWeapon->GetWeaponMesh()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, CurWeapon->GetAttachSocketName());
}
