// Fill out your copyright notice in the Description page of Project Settings.


#include "Soldier.h"
#include "Net/UnrealNetwork.h"

ASoldier::ASoldier()
	: ACreature()
{
	CombatComponent = CreateDefaultSubobject<UCombatComponent>("CombatComponent");
	CombatComponent->OnEquipWeaponSuccess.AddDynamic(this, &ASoldier::OnEquipWeaponSuccess);
	CombatComponent->OnServerEquipWeaponSuccess.AddDynamic(this, &ASoldier::Server_OnEquipWeaponSuccess);
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

	DOREPLIFETIME(ASoldier, CombatComponent);
}

void ASoldier::EquipWeapon(AWeapon* Weapon)
{
	CombatComponent->EquipWeapon(Weapon);
}

void ASoldier::OnEquipWeaponSuccess(AWeapon* Weapon)
{
	if(Weapon == nullptr)
		return;

	CombatComponent->GetCurWeapon()->GetWeaponMesh()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, CombatComponent->GetCurWeapon()->GetAttachSocketName());
}

void ASoldier::Server_OnEquipWeaponSuccess_Implementation(AWeapon* Weapon)
{	
	CombatComponent->GetCurWeapon()->GetWeaponMesh()->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, CombatComponent->GetCurWeapon()->GetAttachSocketName());
}
