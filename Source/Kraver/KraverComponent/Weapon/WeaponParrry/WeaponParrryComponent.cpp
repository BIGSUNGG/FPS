// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponParrryComponent.h"
#include "Kraver/Weapon/Weapon.h"

void UWeaponParrryComponent::BeginPlay()
{
	UWeaponComponent::BeginPlay();

	if(OwnerWeapon == nullptr)
		return;

	OwnerWeapon->OnSubAttackStart.AddDynamic(this, &ThisClass::OnSubAttackStartEvent);
	OwnerWeapon->OnSubAttackEnd.AddDynamic(this, &ThisClass::OnSubAttackEndEvent);
}

void UWeaponParrryComponent::OnSubAttackStartEvent()
{
}

void UWeaponParrryComponent::OnSubAttackEndEvent()
{
}
