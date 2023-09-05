// Fill out your copyright notice in the Description page of Project Settings.


#include "AttachmentScopeComponent.h"
#include Weapon_h

UAttachmentScopeComponent::UAttachmentScopeComponent()
{
	ScopeMesh = CreateDefaultSubobject<UStaticMeshComponent>("Scope");
	ScopeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void UAttachmentScopeComponent::BeginPlay()
{
	Super::BeginPlay();

	ScopeMesh->SetStaticMesh(ScopeStaticMesh);
	ScopeMesh->AttachToComponent(OwnerWeapon->GetTppWeaponMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale, ScopeSocketName);
	ScopeMesh->SetRelativeLocation(ScopeRelativeLocation);

}

void UAttachmentScopeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
