// Fill out your copyright notice in the Description page of Project Settings.


#include "AttachmentMagazineComponent.h"
#include Weapon_h

UAttachmentMagazineComponent::UAttachmentMagazineComponent()
{
	MagazineMesh = CreateDefaultSubobject<UStaticMeshComponent>("Magazine");
	MagazineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void UAttachmentMagazineComponent::BeginPlay()
{
	Super::BeginPlay();

	MagazineMesh->SetStaticMesh(MagazineStaticMesh);
	MagazineMesh->AttachToComponent(OwnerWeapon->GetTppWeaponMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, MagazineSocketName);
	MagazineMesh->SetRelativeLocation(MagazineRelativeLocation);

}

void UAttachmentMagazineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}