// Fill out your copyright notice in the Description page of Project Settings.


#include "AttachmentWeaponComponent.h"
#include "Kraver/Weapon/Weapon.h"

UAttachmentWeaponComponent::UAttachmentWeaponComponent()
{

}

void UAttachmentWeaponComponent::BeginPlay()
{
	OwnerWeapon = Cast<AWeapon>(GetOwner());
}

void UAttachmentWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

}

void UAttachmentWeaponComponent::OnMakeWeaponPrimitiveInfoDeleEvent(FWeaponPrimitiveInfo& WeaponPrimitiveInfo)
{

}
