// Fill out your copyright notice in the Description page of Project Settings.


#include "RpcComponent.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
URpcComponent::URpcComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void URpcComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	if(OwnerCharacter == nullptr)
		KR_LOG(Fatal, TEXT("Owner is not character"));
}


// Called every frame
void URpcComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void URpcComponent::OwningOtherActor(AActor* Actor)
{
	Actor->SetOwner(GetOwner());
	if (GetOwner()->HasAuthority() == false)
		Server_OwningOtherActor(Actor);
}

void URpcComponent::SetSimulatedPhysics(UPrimitiveComponent* Component, bool bSimulated)
{
	if (GetOwner()->HasAuthority() == false)
		Server_SetSimulatedPhysics(Component,bSimulated);
	else
		Multicast_SetSimulatedPhysics(Component, bSimulated);
}

void URpcComponent::AttachComponentToComponent(USceneComponent* Child, USceneComponent* Parent, FName BoneName)
{
	Child->AttachToComponent(Parent, FAttachmentTransformRules::SnapToTargetIncludingScale, BoneName);
	if (GetOwner()->HasAuthority() == false)
		Server_AttachComponentToComponent(Child,Parent, BoneName);
	else
		Multicast_AttachComponentToComponent(Child,Parent,BoneName);
}

void URpcComponent::DetachComponentFromComponent(USceneComponent* Child)
{
	if (GetOwner()->HasAuthority())
		Multicast_DetachComponentFromComponent(Child);
	else
		Server_DetachComponentFromComponent(Child);
}

void URpcComponent::SetPhysicsLinearVelocity(UPrimitiveComponent* Component, FVector Velocity)
{
	if (GetOwner()->HasAuthority() == false)
		Server_SetPhysicsLinearVelocity(Component,Velocity);
	else
		Multicast_SetPhysicsLinearVelocity(Component,Velocity);
}

void URpcComponent::AddImpulse(UPrimitiveComponent* Component, FVector Direction, FName BoneName, bool bVelChange)
{
	if (GetOwner()->HasAuthority() == false)
		Server_AddImpulse(Component, Direction, BoneName, bVelChange);
	else
		Multicast_AddImpulse(Component, Direction, BoneName, bVelChange);
}

void URpcComponent::AddImpulseAtLocation(UPrimitiveComponent* Component, FVector Direction, FVector Location, FName BoneName /*= NAME_None*/)
{
	if (GetOwner()->HasAuthority() == false)
		Server_AddImpulseAtLocation(Component, Direction, Location, BoneName);
	else
		Multicast_AddImpulseAtLocation(Component, Direction, Location, BoneName);
}

void URpcComponent::SetComponentLocation(UPrimitiveComponent* Component, FVector Location)
{
	Component->SetWorldLocation(Location);
	if (GetOwner()->HasAuthority() == false)
		Server_SetComponentLocation(Component,Location);
}

void URpcComponent::SetComponentRotation(UPrimitiveComponent* Component, FRotator Rotation)
{
	Component->SetWorldRotation(Rotation);
	if (GetOwner()->HasAuthority() == false)
		Server_SetComponentRotation(Component, Rotation);
}

void URpcComponent::SetCharacterWalkSpeed(ACharacter* Character, float Speed)
{
	Character->GetCharacterMovement()->MaxWalkSpeed = Speed;
	if (GetOwner()->HasAuthority() == false)
		Server_SetCharacterWalkSpeed(Character, Speed);
}

void URpcComponent::Montage_Play(USkeletalMeshComponent* Mesh, UAnimMontage* Montage, float Speed /*= 1.f*/)
{
	Mesh->GetAnimInstance()->Montage_Play(Montage, Speed);
	if(GetOwner()->HasAuthority() == false)
		Server_Montage_Play(Mesh, Montage, Speed);
	else
		Multicast_Montage_Play(Mesh, Montage, Speed);
}

void URpcComponent::Montage_Stop(USkeletalMeshComponent* Mesh, UAnimMontage* Montage, float Speed /*= 0.f*/)
{
	Mesh->GetAnimInstance()->Montage_Stop(Speed, Montage);
	if (GetOwner()->HasAuthority() == false)
		Server_Montage_Stop(Mesh, Montage, Speed);
	else
		Multicast_Montage_Stop(Mesh, Montage, Speed);
}

void URpcComponent::SetCollisionProfileName(UPrimitiveComponent* Component, FName ProfileName)
{
	if(GetOwner()->HasAuthority())
		Multicast_SetCollisionProfileName(Component,ProfileName);
	else
		Server_SetCollisionProfileName(Component,ProfileName);
}	

void URpcComponent::SpawnNiagaraAtLocation(UObject* WorldContextObject, class UNiagaraSystem* SystemTemplate, FVector Location, FRotator Rotation /*= FRotator::ZeroRotator*/, FVector Scale /*= FVector(1.f)*/, bool bAutoDestroyNiagara /*= true*/, bool bAutoActivateNiagara /*= true*/, ENCPoolMethod PoolingMethod /*= ENCPoolMethod::None*/, bool bPreCullCheck /*= true*/)
{
	if(GetOwner()->HasAuthority())
		Multicast_SpawnNiagaraAtLocation(WorldContextObject, SystemTemplate, Location, Rotation, Scale, bAutoDestroyNiagara, bAutoActivateNiagara, PoolingMethod, bPreCullCheck);
	else
		Server_SpawnNiagaraAtLocation(WorldContextObject, SystemTemplate, Location, Rotation, Scale, bAutoDestroyNiagara, bAutoActivateNiagara, PoolingMethod, bPreCullCheck);
}

void URpcComponent::SetCollisionEnabled(UPrimitiveComponent* Object, ECollisionEnabled::Type Value)
{
	if (GetOwner()->HasAuthority())
		Multicast_SetCollisionEnabled(Object, Value);
	else
		Server_SetCollisionEnabled(Object, Value);
}

void URpcComponent::SetHiddenInGame(USceneComponent* Component, bool NewHidden)
{
	Server_SetHiddenInGame(Component, NewHidden);	
}

void URpcComponent::SetVelocity(FVector Value)
{
	OwnerCharacter->GetCharacterMovement()->Velocity = Value;
	Server_SetVelocity(Value);
}

void URpcComponent::SetPendingLaunchVelocity(FVector Value)
{
	OwnerCharacter->GetCharacterMovement()->PendingLaunchVelocity = Value;
	Server_SetPendingLaunchVelocity(Value);
}

void URpcComponent::SetGroundFriction(float Value)
{
	OwnerCharacter->GetCharacterMovement()->GroundFriction = Value;
	Server_SetGroundFriction(Value);
}

void URpcComponent::SetBrakingDecelerationWalking(float Value)
{
	OwnerCharacter->GetCharacterMovement()->BrakingDecelerationWalking = Value;
	Server_SetBrakingDecelerationWalking(Value);
}

void URpcComponent::SetGravityScale(float Value)
{
	OwnerCharacter->GetCharacterMovement()->GravityScale = Value;
	Server_SetGravityScale(Value);
}

void URpcComponent::AddAngularImpulseInDegrees(UPrimitiveComponent* Component, FVector Impulse, FName BoneName /*= NAME_None*/, bool bVelChange /*= false*/)
{
	if (GetOwner()->HasAuthority() == false)
		Server_AddAngularImpulseInDegrees(Component, Impulse, BoneName, bVelChange);
	else
		Multicast_AddAngularImpulseInDegrees(Component, Impulse, BoneName, bVelChange);
}

void URpcComponent::SetOwnerLocation(FVector Location)
{
	OwnerCharacter->SetActorLocation(Location);
	Server_SetOwnerLocation(Location);
}

void URpcComponent::SetOwnerRotation(FRotator Rotation)
{
	OwnerCharacter->SetActorRotation(Rotation);
	Server_SetOwnerRotation(Rotation);
}

void URpcComponent::RegistCurMovement()
{
	Server_SetOwnerLocation(OwnerCharacter->GetActorLocation());
	Server_SetOwnerRotation(OwnerCharacter->GetActorRotation());
	Server_SetVelocity(OwnerCharacter->GetCharacterMovement()->Velocity);
	Server_SetPendingLaunchVelocity(OwnerCharacter->GetCharacterMovement()->PendingLaunchVelocity);
}

void URpcComponent::Server_SetPhysicsLinearVelocity_Implementation(UPrimitiveComponent* Component, FVector Velocity)
{
	Multicast_SetPhysicsLinearVelocity(Component,Velocity);
}

void URpcComponent::Server_AddImpulse_Implementation(UPrimitiveComponent* Component, FVector Direction, FName BoneName, bool bVelChange)
{
	Multicast_AddImpulse(Component,Direction,BoneName,bVelChange);
}

void URpcComponent::Server_AddImpulseAtLocation_Implementation(UPrimitiveComponent* Component, FVector Direction, FVector Location, FName BoneName /*= NAME_None*/)
{
	Multicast_AddImpulseAtLocation(Component, Direction, Location, BoneName);
}

void URpcComponent::Server_SetSimulatedPhysics_Implementation(UPrimitiveComponent* Component, bool bSimulated)
{
	Multicast_SetSimulatedPhysics(Component,bSimulated);
}

void URpcComponent::Server_OwningOtherActor_Implementation(AActor* Actor)
{
	Actor->SetOwner(GetOwner());
}

void URpcComponent::Server_AttachComponentToComponent_Implementation(USceneComponent* Child, USceneComponent* Parent, FName BoneName)
{
	Child->AttachToComponent(Parent, FAttachmentTransformRules::SnapToTargetIncludingScale, BoneName);
	Multicast_AttachComponentToComponent(Child,Parent, BoneName);
}

void URpcComponent::Server_SetComponentLocation_Implementation(UPrimitiveComponent* Component, FVector Location)
{
	Component->SetWorldLocation(Location);
}

void URpcComponent::Server_SetComponentRotation_Implementation(UPrimitiveComponent* Component, FRotator Rotation)
{
	Component->SetWorldRotation(Rotation);
}

void URpcComponent::Server_DetachComponentFromComponent_Implementation(USceneComponent* Child)
{
	Multicast_DetachComponentFromComponent(Child);
}

void URpcComponent::Server_SetCharacterWalkSpeed_Implementation(ACharacter* Character, float Speed)
{
	Character->GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void URpcComponent::Server_Montage_Play_Implementation(USkeletalMeshComponent* Mesh, UAnimMontage* Montage, float Speed /*= 1.f*/)
{
	Multicast_Montage_Play(Mesh,Montage,Speed);
}

void URpcComponent::Server_Montage_Stop_Implementation(USkeletalMeshComponent* Mesh, UAnimMontage* Montage, float Speed /*= 1.f*/)
{
	Multicast_Montage_Stop(Mesh,Montage,Speed);
}

void URpcComponent::Server_SetCollisionProfileName_Implementation(UPrimitiveComponent* Component, FName ProfileName)
{
	Multicast_SetCollisionProfileName(Component, ProfileName);
}

void URpcComponent::Server_SpawnNiagaraAtLocation_Implementation(UObject* WorldContextObject, class UNiagaraSystem* SystemTemplate, FVector Location, FRotator Rotation /*= FRotator::ZeroRotator*/, FVector Scale /*= FVector(1.f)*/, bool bAutoDestroyNiagara /*= true*/, bool bAutoActivateNiagara /*= true*/, ENCPoolMethod PoolingMethod /*= ENCPoolMethod::None*/, bool bPreCullCheck /*= true*/)
{
	Multicast_SpawnNiagaraAtLocation(WorldContextObject, SystemTemplate, Location, Rotation, Scale, bAutoDestroyNiagara, bAutoActivateNiagara, PoolingMethod, bPreCullCheck);
}

void URpcComponent::Server_SetCollisionEnabled_Implementation(UPrimitiveComponent* Object, ECollisionEnabled::Type Value)
{
	Multicast_SetCollisionEnabled(Object,Value);
}

void URpcComponent::Server_SetHiddenInGame_Implementation(USceneComponent* Component, bool NewHidden)
{
	Multicast_SetHiddenInGame(Component,NewHidden);
}

void URpcComponent::Server_SetVelocity_Implementation(FVector Value)
{
	OwnerCharacter->GetCharacterMovement()->Velocity = Value;
}

void URpcComponent::Server_SetPendingLaunchVelocity_Implementation(FVector Value)
{
	OwnerCharacter->GetCharacterMovement()->PendingLaunchVelocity = Value;
}

void URpcComponent::Server_SetGroundFriction_Implementation(float Value)
{
	OwnerCharacter->GetCharacterMovement()->GroundFriction = Value;
}

void URpcComponent::Server_SetBrakingDecelerationWalking_Implementation(float Value)
{
	OwnerCharacter->GetCharacterMovement()->BrakingDecelerationWalking = Value;
}

void URpcComponent::Server_SetGravityScale_Implementation(float Value)
{
	OwnerCharacter->GetCharacterMovement()->GravityScale = Value;
}

void URpcComponent::Server_AddAngularImpulseInDegrees_Implementation(UPrimitiveComponent* Component, FVector Impulse, FName BoneName /*= NAME_None*/, bool bVelChange /*= false*/)
{
	Multicast_AddAngularImpulseInDegrees(Component,Impulse,BoneName,bVelChange);
}

void URpcComponent::Server_SetOwnerLocation_Implementation(FVector Location)
{
	OwnerCharacter->SetActorLocation(Location);
}

void URpcComponent::Server_SetOwnerRotation_Implementation(FRotator Rotation)
{
	OwnerCharacter->SetActorRotation(Rotation);
}

void URpcComponent::Multicast_AttachComponentToComponent_Implementation(USceneComponent* Child, USceneComponent* Parent, FName BoneName)
{
	Child->AttachToComponent(Parent, FAttachmentTransformRules::SnapToTargetIncludingScale, BoneName);
}

void URpcComponent::Multicast_SetSimulatedPhysics_Implementation(UPrimitiveComponent* Component, bool bSimulated)
{
	Component->SetSimulatePhysics(bSimulated);
}

void URpcComponent::Multicast_AddImpulseAtLocation_Implementation(UPrimitiveComponent* Component, FVector Direction, FVector Location, FName BoneName /*= NAME_None*/)
{
	Component->AddImpulseAtLocation(Direction,Location,BoneName);
}

void URpcComponent::Multicast_SetCollisionProfileName_Implementation(UPrimitiveComponent* Component, FName ProfileName)
{
	Component->SetCollisionProfileName(ProfileName);
}

void URpcComponent::Multicast_Montage_Play_Implementation(USkeletalMeshComponent* Mesh, UAnimMontage* Montage, float Speed /*= 1.f*/)
{
	Mesh->GetAnimInstance()->Montage_Play(Montage,Speed);
}

void URpcComponent::Multicast_Montage_Stop_Implementation(USkeletalMeshComponent* Mesh, UAnimMontage* Montage, float Speed /*= 1.f*/)
{
	Mesh->GetAnimInstance()->Montage_Stop(Speed, Montage);
}

void URpcComponent::Multicast_SpawnNiagaraAtLocation_Implementation(UObject* WorldContextObject, class UNiagaraSystem* SystemTemplate, FVector Location, FRotator Rotation /*= FRotator::ZeroRotator*/, FVector Scale /*= FVector(1.f)*/, bool bAutoDestroyNiagara /*= true*/, bool bAutoActivateNiagara /*= true*/, ENCPoolMethod PoolingMethod /*= ENCPoolMethod::None*/, bool bPreCullCheck /*= true*/)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(WorldContextObject, SystemTemplate, Location, Rotation, Scale, bAutoDestroyNiagara, bAutoActivateNiagara, PoolingMethod, bPreCullCheck);
}

void URpcComponent::Multicast_SetPhysicsLinearVelocity_Implementation(UPrimitiveComponent* Component, FVector Velocity)
{
	Component->SetPhysicsLinearVelocity(Velocity);
}

void URpcComponent::Multicast_AddImpulse_Implementation(UPrimitiveComponent* Component, FVector Direction, FName BoneName /*= NAME_None*/, bool bVelChange /*= false*/)
{
	Component->AddImpulse(Direction, BoneName, bVelChange);
}

void URpcComponent::Multicast_DetachComponentFromComponent_Implementation(USceneComponent* Child)
{
	Child->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void URpcComponent::Multicast_SetCollisionEnabled_Implementation(UPrimitiveComponent* Object, ECollisionEnabled::Type Value)
{
	Object->SetCollisionEnabled(Value);
}

void URpcComponent::Multicast_SetHiddenInGame_Implementation(USceneComponent* Component, bool NewHidden)
{	
	Component->SetHiddenInGame(NewHidden);
}

void URpcComponent::Multicast_AddAngularImpulseInDegrees_Implementation(UPrimitiveComponent* Component, FVector Impulse, FName BoneName /*= NAME_None*/, bool bVelChange /*= false*/)
{
	Component->AddAngularImpulseInDegrees(Impulse,BoneName,bVelChange);
}
