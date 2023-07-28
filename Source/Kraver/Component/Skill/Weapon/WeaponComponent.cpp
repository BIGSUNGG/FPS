// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"
#include "Kraver/Actor/Weapon/Weapon.h"

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

	OwnerWeapon->OnBeforeAttack.AddDynamic(this, &UWeaponComponent::OnBeforeAttackEvent);
	OwnerWeapon->OnAddOnDelegate.AddDynamic(this, &UWeaponComponent::OnAddOnDelegateEvent);
	OwnerWeapon->OnRemoveOnDelegate.AddDynamic(this, &UWeaponComponent::OnRemoveOnDelegateEvent);

}

// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWeaponComponent::OnAddOnDelegateEvent(UObject* Object)
{

}

void UWeaponComponent::OnRemoveOnDelegateEvent(UObject* Object)
{

}

void UWeaponComponent::OnBeforeAttackEvent()
{

}

ACreature* UWeaponComponent::GetOwnerCreature()
{
	return OwnerWeapon->GetOwnerCreature();
}

