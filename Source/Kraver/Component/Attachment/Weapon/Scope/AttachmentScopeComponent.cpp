// Fill out your copyright notice in the Description page of Project Settings.


#include "AttachmentScopeComponent.h"
#include "Kraver/Actor/Weapon/Weapon.h"

UAttachmentScopeComponent::UAttachmentScopeComponent()
{
	ScopeMesh = CreateDefaultSubobject<UStaticMeshComponent>("ScopeMesh");
	ScopeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void UAttachmentScopeComponent::BeginPlay()
{
	Super::BeginPlay();

	ScopeMesh->SetStaticMesh(ScopeStaticMesh);
	ScopeMesh->AttachToComponent(OwnerWeapon->GetTppWeaponPrimitiveInfo()["Root"],FAttachmentTransformRules::SnapToTargetIncludingScale, ScopeSocketName);
	ScopeMesh->SetRelativeLocation(ScopeRelativeLocation);

	OwnerWeapon->AddWeaponPrimitive("Scope", ScopeMesh);
}

void UAttachmentScopeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
