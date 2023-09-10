// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponAdsComponent.h"
#include Weapon_h

void UWeaponAdsComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerWeapon->OnSubAttackStart.AddDynamic(this, &ThisClass::OnSubAttackStartEvent);
	OwnerWeapon->OnSubAttackEnd.AddDynamic(this, &ThisClass::OnSubAttackEndEvent);
}

void UWeaponAdsComponent::OnSubAttackStartEvent()
{
	if (ScopeMaterial)
	{
		UPrimitiveComponent* ScopeMesh = OwnerWeapon->GetFppWeaponPrimitiveInfo()["Scope"];
		OriginScopeMaterial = ScopeMesh->GetMaterial(ScopeMaterialIndex);
		ScopeMesh->SetMaterial(ScopeMaterialIndex, ScopeMaterial);
	}
}

void UWeaponAdsComponent::OnSubAttackEndEvent()
{
	if (OriginScopeMaterial)
	{
		UPrimitiveComponent* ScopeMesh = OwnerWeapon->GetFppWeaponPrimitiveInfo()["Scope"];
		ScopeMesh->SetMaterial(ScopeMaterialIndex, OriginScopeMaterial);
	}
}
