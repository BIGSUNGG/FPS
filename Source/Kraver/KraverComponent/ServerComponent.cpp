// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerComponent.h"
#include "GameFramework/Character.h"

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
	if (GetOwner()->HasAuthority() == false)
		Server_OwningOtherActor(Actor);
}

void UServerComponent::SetSimulatedPhysics(UPrimitiveComponent* Component, bool bSimulated)
{
	Component->SetSimulatePhysics(bSimulated);
	if (GetOwner()->HasAuthority() == false)
		Server_SetSimulatedPhysics(Component,bSimulated);
	else
		Multicast_SetSimulatedPhysics(Component, bSimulated);
}

void UServerComponent::AttachComponentToComponent(USceneComponent* Child, USceneComponent* Parent, FName BoneName)
{
	Child->AttachToComponent(Parent, FAttachmentTransformRules::SnapToTargetIncludingScale, BoneName);
	if (GetOwner()->HasAuthority() == false)
		Server_AttachComponentToComponent(Child,Parent, BoneName);
	else
		Multicast_AttachComponentToComponent(Child,Parent,BoneName);
}

void UServerComponent::DetachComponentFromComponent(USceneComponent* Child)
{
	Child->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	if (GetOwner()->HasAuthority() == false)
		Server_DetachComponentFromComponent(Child);
}

void UServerComponent::SetPhysicsLinearVelocity(UPrimitiveComponent* Component, FVector Velocity)
{
	Component->SetPhysicsLinearVelocity(Velocity);
	if (GetOwner()->HasAuthority() == false)
		Server_SetPhysicsLinearVelocity(Component,Velocity);
}

void UServerComponent::AddImpulse(UPrimitiveComponent* Component, FVector Direction, FName BoneName, bool bVelChange)
{
	Server_AddImpulse(Component,Direction,BoneName,bVelChange);
}

void UServerComponent::AddImpulseAtLocation(UPrimitiveComponent* Component, FVector Direction, FVector Location, FName BoneName /*= NAME_None*/)
{
	if (GetOwner()->HasAuthority() == false)
		Server_AddImpulseAtLocation(Component, Direction, Location, BoneName);
	else
		Multicast_AddImpulseAtLocation(Component, Direction, Location, BoneName);
}

void UServerComponent::SetLocation(UPrimitiveComponent* Component, FVector Location)
{
	Component->SetWorldLocation(Location);
	if (GetOwner()->HasAuthority() == false)
		Server_SetLocation(Component,Location);
}

void UServerComponent::SetRotation(UPrimitiveComponent* Component, FRotator Rotation)
{
	Component->SetWorldRotation(Rotation);
	if (GetOwner()->HasAuthority() == false)
		Server_SetRotation(Component, Rotation);
}

void UServerComponent::SetCharacterWalkSpeed(ACharacter* Character, float Speed)
{
	Character->GetCharacterMovement()->MaxWalkSpeed = Speed;
	if (GetOwner()->HasAuthority() == false)
		Server_SetCharacterWalkSpeed(Character, Speed);
}

void UServerComponent::PlayMontage(UAnimInstance* Anim, UAnimMontage* Montage, float Speed /*= 1.f*/)
{
	Anim->Montage_Play(Montage,Speed);
	if(GetOwner()->HasAuthority() == false)
		Server_PlayMontage(Anim, Montage, Speed);
	else
		Client_PlayMontage(Anim, Montage, Speed);
}

void UServerComponent::Server_SetPhysicsLinearVelocity_Implementation(UPrimitiveComponent* Component, FVector Velocity)
{
	Component->SetPhysicsLinearVelocity(Velocity);
}

void UServerComponent::Server_AddImpulse_Implementation(UPrimitiveComponent* Component, FVector Direction, FName BoneName, bool bVelChange)
{
	Component->AddImpulse(Direction, BoneName, bVelChange);
}

void UServerComponent::Server_AddImpulseAtLocation_Implementation(UPrimitiveComponent* Component, FVector Direction, FVector Location, FName BoneName /*= NAME_None*/)
{
	Multicast_AddImpulseAtLocation(Component, Direction, Location, BoneName);
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

void UServerComponent::Server_SetCharacterWalkSpeed_Implementation(ACharacter* Character, float Speed)
{
	Character->GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void UServerComponent::Server_PlayMontage_Implementation(UAnimInstance* Anim, UAnimMontage* Montage, float Speed /*= 1.f*/)
{
	Client_PlayMontage(Anim,Montage,Speed);
}

void UServerComponent::Multicast_AttachComponentToComponent_Implementation(USceneComponent* Child, USceneComponent* Parent, FName BoneName)
{
	Child->AttachToComponent(Parent, FAttachmentTransformRules::SnapToTargetIncludingScale, BoneName);
}

void UServerComponent::Multicast_SetSimulatedPhysics_Implementation(UPrimitiveComponent* Component, bool bSimulated)
{
	Component->SetSimulatePhysics(bSimulated);
}

void UServerComponent::Multicast_AddImpulseAtLocation_Implementation(UPrimitiveComponent* Component, FVector Direction, FVector Location, FName BoneName /*= NAME_None*/)
{
	Component->AddImpulseAtLocation(Direction,Location,BoneName);
}

void UServerComponent::Client_PlayMontage_Implementation(UAnimInstance* Anim, UAnimMontage* Montage, float Speed /*= 1.f*/)
{
	Anim->Montage_Play(Montage, Speed);
}
