// Fill out your copyright notice in the Description page of Project Settings.


#include "AttachmentScopeComponent.h"
#include "Kraver/Weapon/Weapon.h"

UAttachmentScopeComponent::UAttachmentScopeComponent()
{
	ScopeMesh = CreateDefaultSubobject<UStaticMeshComponent>("ScopeMesh");

}

void UAttachmentScopeComponent::BeginPlay()
{
	UAttachmentWeaponComponent::BeginPlay();

	ScopeMesh->SetStaticMesh(ScopeStaticMesh);
	ScopeMesh->AttachToComponent(OwnerWeapon->GetWeaponPrimitiveInfo()["Root"],FAttachmentTransformRules::SnapToTargetIncludingScale, ScopeSocketName);
	ScopeMesh->SetRelativeLocation(ScopeRelativeLocation);

	OwnerWeapon->AddWeaponPrimitive("Scope", ScopeMesh);
}

void UAttachmentScopeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UAttachmentWeaponComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
