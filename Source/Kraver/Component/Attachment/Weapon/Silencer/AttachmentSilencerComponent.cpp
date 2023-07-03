// Fill out your copyright notice in the Description page of Project Settings.


#include "AttachmentSilencerComponent.h"
#include "Kraver/Actor/Weapon/Weapon.h"

UAttachmentSilencerComponent::UAttachmentSilencerComponent()
{
	SilencerMesh = CreateDefaultSubobject<UStaticMeshComponent>("SilencerMesh");
	SilencerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UAttachmentSilencerComponent::BeginPlay()
{
	UAttachmentWeaponComponent::BeginPlay();

	SilencerMesh->SetStaticMesh(SilencerStaticMesh);
	SilencerMesh->AttachToComponent(OwnerWeapon->GetWeaponPrimitiveInfo()["Root"], FAttachmentTransformRules::SnapToTargetIncludingScale, SilencerSocketName);
	SilencerMesh->SetRelativeLocation(SilencerRelativeLocation);

	OwnerWeapon->AddWeaponPrimitive("Silencer", SilencerMesh);
}

void UAttachmentSilencerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

}
