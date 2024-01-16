// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"
#include Weapon_h

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwnerWeapon = Cast<AWeapon>(GetOwner());
	if (OwnerWeapon == nullptr)
	{
		KR_LOG(Error, TEXT("Owner Actor is not Weapon class"));
		return;
	}

	OwnerWeapon->OnLocalAddOnDelegate.AddDynamic(this, &UWeaponComponent::OnLocal_AddOnDelegateEvent);
	OwnerWeapon->OnLocalRemoveOnDelegate.AddDynamic(this, &UWeaponComponent::OnLocal_RemoveOnDelegateEvent);

	OwnerWeapon->OnServerAddOnDelegate.AddDynamic(this, &UWeaponComponent::OnServer_AddOnDelegateEvent);
	OwnerWeapon->OnServerRemoveOnDelegate.AddDynamic(this, &UWeaponComponent::OnServer_RemoveOnDelegateEvent);

}

// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWeaponComponent::OnLocal_AddOnDelegateEvent(UObject* Object)
{

}

void UWeaponComponent::OnLocal_RemoveOnDelegateEvent(UObject* Object)
{

}

void UWeaponComponent::OnServer_AddOnDelegateEvent(UObject* Object)
{

}

void UWeaponComponent::OnServer_RemoveOnDelegateEvent(UObject* Object)
{

}

ACreature* UWeaponComponent::GetOwnerCreature()
{
	return OwnerWeapon->GetOwnerCreature();
}

