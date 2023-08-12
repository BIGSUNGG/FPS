// Fill out your copyright notice in the Description page of Project Settings.


#include "AttachmentMagazineComponent.h"
#include "Kraver/Actor/Weapon/Weapon.h"

UAttachmentMagazineComponent::UAttachmentMagazineComponent()
{
	MagazineMesh = CreateDefaultSubobject<UStaticMeshComponent>("MagazineMesh");
	MagazineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void UAttachmentMagazineComponent::BeginPlay()
{
	UAttachmentWeaponComponent::BeginPlay();

	MagazineMesh->SetStaticMesh(MagazineStaticMesh);
	MagazineMesh->AttachToComponent(OwnerWeapon->GetTppWeaponPrimitiveInfo()["Root"], FAttachmentTransformRules::SnapToTargetIncludingScale, MagazineSocketName);
	MagazineMesh->SetRelativeLocation(MagazineRelativeLocation);

	OwnerWeapon->AddWeaponPrimitive("Magazine", MagazineMesh);
}

void UAttachmentMagazineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UAttachmentWeaponComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);

}