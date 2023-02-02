// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerComponent.h"

// Sets default values for this component's properties
UServerComponent::UServerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UServerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UServerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UServerComponent::OwningOtherActor(AActor* Actor)
{
	Actor->SetOwner(GetOwner());
	Server_OwningOtherActor(Actor);
}

void UServerComponent::SetSimulatedPhysics(UPrimitiveComponent* Component, bool bSimulated)
{
	Component->SetSimulatePhysics(bSimulated);
	Server_SetSimulatedPhysics(Component,bSimulated);
}

void UServerComponent::AttachComponentToComponent(USceneComponent* Child, USceneComponent* Parent, FName BoneName)
{
	Child->AttachToComponent(Parent, FAttachmentTransformRules::SnapToTargetIncludingScale, BoneName);
	Server_AttachComponentToComponent(Child,Parent, BoneName);
}

void UServerComponent::DetachComponentFromComponent(USceneComponent* Child)
{
	Child->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	Server_DetachComponentFromComponent(Child);
}

void UServerComponent::SetPhysicsLinearVelocity(UPrimitiveComponent* Component, FVector Velocity)
{
	Component->SetPhysicsLinearVelocity(Velocity);
	Server_SetPhysicsLinearVelocity(Component,Velocity);
}

void UServerComponent::AddImpulse(UPrimitiveComponent* Component, FVector Direction, FName BoneName, bool bVelChange)
{
	Component->AddImpulse(Direction, BoneName, bVelChange);
	Server_AddImpulse(Component,Direction,BoneName,bVelChange);
}

void UServerComponent::SetLocation(UPrimitiveComponent* Component, FVector Location)
{
	Component->SetWorldLocation(Location);
	Server_SetLocation(Component,Location);
}

void UServerComponent::SetRotation(UPrimitiveComponent* Component, FRotator Rotation)
{
	Component->SetWorldRotation(Rotation);
	Server_SetRotation(Component, Rotation);
}

void UServerComponent::Server_SetPhysicsLinearVelocity_Implementation(UPrimitiveComponent* Component, FVector Velocity)
{
	Component->SetPhysicsLinearVelocity(Velocity);
}

void UServerComponent::Server_AddImpulse_Implementation(UPrimitiveComponent* Component, FVector Direction, FName BoneName, bool bVelChange)
{
	Component->AddImpulse(Direction, BoneName, bVelChange);
}

void UServerComponent::Server_SetSimulatedPhysics_Implementation(UPrimitiveComponent* Component, bool bSimulated)
{
	Component->SetSimulatePhysics(bSimulated);
	Multicast_SetSimulatedPhysics(Component,bSimulated);
}

void UServerComponent::Server_OwningOtherActor_Implementation(AActor* Actor)
{
	Actor->SetOwner(GetOwner());
}

void UServerComponent::Server_AttachComponentToComponent_Implementation(USceneComponent* Child, USceneComponent* Parent, FName BoneName)
{
	Child->AttachToComponent(Parent, FAttachmentTransformRules::SnapToTargetIncludingScale, BoneName);
	Multicast_AttachComponentToComponent(Child,Parent, BoneName);
}

void UServerComponent::Server_SetLocation_Implementation(UPrimitiveComponent* Component, FVector Location)
{
	Component->SetWorldLocation(Location);
}

void UServerComponent::Server_SetRotation_Implementation(UPrimitiveComponent* Component, FRotator Rotation)
{
	Component->SetWorldRotation(Rotation);
}

void UServerComponent::Server_DetachComponentFromComponent_Implementation(USceneComponent* Child)
{
	Child->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void UServerComponent::Multicast_AttachComponentToComponent_Implementation(USceneComponent* Child, USceneComponent* Parent, FName BoneName)
{
	Child->AttachToComponent(Parent, FAttachmentTransformRules::SnapToTargetIncludingScale, BoneName);
}

void UServerComponent::Multicast_SetSimulatedPhysics_Implementation(UPrimitiveComponent* Component, bool bSimulated)
{
	Component->SetSimulatePhysics(bSimulated);
}
