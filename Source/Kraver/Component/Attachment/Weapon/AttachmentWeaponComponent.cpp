// Fill out your copyright notice in the Description page of Project Settings.


#include "AttachmentWeaponComponent.h"
#include "Kraver/Actor/Weapon/Weapon.h"

UAttachmentWeaponComponent::UAttachmentWeaponComponent()
{

}

void UAttachmentWeaponComponent::BeginPlay()
{
	UAttachmentComponent::BeginPlay();

	OwnerWeapon = Cast<AWeapon>(GetOwner());
	if (!OwnerWeapon)
	{
		KR_LOG(Error, TEXT("Owner is not Weapon"));
		return;
	}

	OwnerWeapon->OnMakeNewPrimitiveInfo.AddDynamic(this, &UAttachmentWeaponComponent::OnMakeWeaponPrimitiveInfoEvent);
	OwnerWeapon->OnAddOnDelegate.AddDynamic(this, &UAttachmentWeaponComponent::OnAddOnDelegateEvent);
	OwnerWeapon->OnRemoveOnDelegate.AddDynamic(this, &UAttachmentWeaponComponent::OnRemoveOnDelegateEvent);
}

void UAttachmentWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UAttachmentComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAttachmentWeaponComponent::OnMakeWeaponPrimitiveInfoEvent(int Index)
{

}

void UAttachmentWeaponComponent::OnAddOnDelegateEvent(UObject* Object)
{

}

void UAttachmentWeaponComponent::OnRemoveOnDelegateEvent(UObject* Object)
{

}
