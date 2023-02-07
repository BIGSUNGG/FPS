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

}

void ASoldier::EquipWeapon(AWeapon* Weapon)
{
	CombatComponent->EquipWeapon(Weapon);
}
