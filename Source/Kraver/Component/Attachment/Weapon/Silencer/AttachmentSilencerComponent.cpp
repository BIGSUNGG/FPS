// Fill out your copyright notice in the Description page of Project Settings.


#include "AttachmentSilencerComponent.h"
#include Weapon_h

UAttachmentSilencerComponent::UAttachmentSilencerComponent()
{
	SilencerMesh = CreateDefaultSubobject<UStaticMeshComponent>("Silencer");
	SilencerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UAttachmentSilencerComponent::BeginPlay()
{
	Super::BeginPlay();

	SilencerMesh->SetStaticMesh(SilencerStaticMesh);
	SilencerMesh->AttachToComponent(OwnerWeapon->GetTppWeaponMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SilencerSocketName);
	SilencerMesh->SetRelativeLocation(SilencerRelativeLocation);

}

void UAttachmentSilencerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

}
