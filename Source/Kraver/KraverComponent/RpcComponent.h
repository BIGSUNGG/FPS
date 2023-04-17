// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "Components/ActorComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Engine/EngineTypes.h"
#include "RpcComponent.generated.h"

// 서버에서 호출되는 함수를 가지는 컴포넌트
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAVER_API URpcComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URpcComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	ACharacter* OwnerCharacter;

public:
	void OwningOtherActor(AActor* Actor);
	void SetSimulatedPhysics(UPrimitiveComponent* Component, bool bSimulated);
	void AttachComponentToComponent(USceneComponent* Child, USceneComponent* Parent, FName BoneName = NAME_None);
	void DetachComponentFromComponent(USceneComponent* Child);
	void SetPhysicsLinearVelocity(UPrimitiveComponent* Component, FVector Velocity);
	void AddImpulse(UPrimitiveComponent* Component, FVector Direction, FName BoneName = NAME_None, bool bVelChange = false);
	void AddImpulseAtLocation(UPrimitiveComponent* Component, FVector Direction, FVector Location, FName BoneName = NAME_None);
	void SetComponentLocation(UPrimitiveComponent* Component, FVector Location);
	void SetComponentRotation(UPrimitiveComponent* Component, FRotator Location);
	void SetCharacterWalkSpeed(ACharacter* Character, float Speed);
	void Montage_Play(USkeletalMeshComponent* Mesh, UAnimMontage* Montage, float Speed = 1.f);
	void Montage_Stop(USkeletalMeshComponent* Mesh, UAnimMontage* Montage, float Speed = 0.f);
	void SetCollisionProfileName(UPrimitiveComponent* Component, FName ProfileName);
	void SpawnNiagaraAtLocation(UObject* WorldContextObject, class UNiagaraSystem* SystemTemplate, FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.f), bool bAutoDestroyNiagara = true, bool bAutoActivateNiagara = true, ENCPoolMethod PoolingMethod = ENCPoolMethod::None, bool bPreCullCheck = true);
	void SetCollisionEnabled(UPrimitiveComponent* Object, ECollisionEnabled::Type Value);
	void SetHiddenInGame(USceneComponent* Component, bool NewHidden);
	void SetVelocity(FVector Value); 
	void SetPendingLaunchVelocity(FVector Value);
	void SetGroundFriction(float Value);
	void SetBrakingDecelerationWalking(float Value);
	void SetGravityScale(float Value);
	void AddAngularImpulseInDegrees(UPrimitiveComponent* Component, FVector Impulse, FName BoneName = NAME_None, bool bVelChange = false);
	void SetOwnerLocation(FVector Location);
	void SetOwnerRotation(FRotator Rotation);

	void RegistCurMovement();

private:
	// Server
	UFUNCTION(Server, Reliable)
		void Server_OwningOtherActor(AActor* Actor);
	UFUNCTION(Server, Reliable)
		void Server_SetSimulatedPhysics(UPrimitiveComponent* Component, bool bSimulated);
	UFUNCTION(Server, Reliable)
		void Server_AttachComponentToComponent(USceneComponent* Child, USceneComponent* Parent, FName BoneName = NAME_None);
	UFUNCTION(Server, Reliable)
		void Server_DetachComponentFromComponent(USceneComponent* Child);
	UFUNCTION(Server, Reliable)
		void Server_SetPhysicsLinearVelocity(UPrimitiveComponent* Component, FVector Velocity);
	UFUNCTION(Server, Reliable)
		void Server_AddImpulse(UPrimitiveComponent* Component, FVector Direction, FName BoneName = NAME_None, bool bVelChange = false);
	UFUNCTION(Server, Reliable)
		void Server_AddImpulseAtLocation(UPrimitiveComponent* Component, FVector Direction, FVector Location, FName BoneName = NAME_None);
	UFUNCTION(Server, Reliable)
		void Server_SetComponentLocation(UPrimitiveComponent* Component, FVector Location);
	UFUNCTION(Server, Reliable)
		void Server_SetComponentRotation(UPrimitiveComponent* Component, FRotator Rotation);
	UFUNCTION(Server, Reliable)
		void Server_SetCharacterWalkSpeed(ACharacter* Character, float Speed);
	UFUNCTION(Server, Reliable)
		void Server_Montage_Play(USkeletalMeshComponent* Mesh, UAnimMontage* Montage, float Speed = 1.f);
	UFUNCTION(Server, Reliable)
		void Server_Montage_Stop(USkeletalMeshComponent* Mesh, UAnimMontage* Montage, float Speed = 1.f);
	UFUNCTION(Server, Reliable)
		void Server_SetCollisionProfileName(UPrimitiveComponent* Component, FName ProfileName);
	UFUNCTION(Server, Reliable)
		void Server_SpawnNiagaraAtLocation(UObject* WorldContextObject, class UNiagaraSystem* SystemTemplate, FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.f), bool bAutoDestroyNiagara = true, bool bAutoActivateNiagara = true, ENCPoolMethod PoolingMethod = ENCPoolMethod::None, bool bPreCullCheck = true);
	UFUNCTION(Server, Reliable)
		void Server_SetCollisionEnabled(UPrimitiveComponent* Object, ECollisionEnabled::Type Value);
	UFUNCTION(Server, Reliable)
		void Server_SetHiddenInGame(USceneComponent* Component, bool NewHidden);
	UFUNCTION(Server, Reliable)
		void Server_SetVelocity(FVector Value);
	UFUNCTION(Server, Reliable)
		void Server_SetPendingLaunchVelocity(FVector Value);
	UFUNCTION(Server, Reliable)
		void Server_SetGroundFriction(float Value);
	UFUNCTION(Server, Reliable)
		void Server_SetBrakingDecelerationWalking(float Value);
	UFUNCTION(Server, Reliable)
		void Server_SetGravityScale(float Value);
	UFUNCTION(Server, Reliable)
		void Server_AddAngularImpulseInDegrees(UPrimitiveComponent* Component, FVector Impulse, FName BoneName = NAME_None, bool bVelChange = false);
	UFUNCTION(Server, Reliable)
		void Server_SetOwnerLocation(FVector Location);
	UFUNCTION(Server, Reliable)
		void Server_SetOwnerRotation(FRotator Rotation);

private:
	// Multicast
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_AttachComponentToComponent(USceneComponent* Child, USceneComponent* Parent, FName BoneName = NAME_None);
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_SetSimulatedPhysics(UPrimitiveComponent* Component, bool bSimulated);
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_AddImpulseAtLocation(UPrimitiveComponent* Component, FVector Direction, FVector Location, FName BoneName = NAME_None);
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_SetCollisionProfileName(UPrimitiveComponent* Component, FName ProfileName);
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_Montage_Play(USkeletalMeshComponent* Mesh, UAnimMontage* Montage, float Speed = 1.f);
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_Montage_Stop(USkeletalMeshComponent* Mesh, UAnimMontage* Montage, float Speed = 1.f);
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_SpawnNiagaraAtLocation(UObject* WorldContextObject, class UNiagaraSystem* SystemTemplate, FVector Location, FRotator Rotation = FRotator::ZeroRotator, FVector Scale = FVector(1.f), bool bAutoDestroyNiagara = true, bool bAutoActivateNiagara = true, ENCPoolMethod PoolingMethod = ENCPoolMethod::None, bool bPreCullCheck = true);
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_SetPhysicsLinearVelocity(UPrimitiveComponent* Component, FVector Velocity);
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_AddImpulse(UPrimitiveComponent* Component, FVector Direction, FName BoneName = NAME_None, bool bVelChange = false);
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_DetachComponentFromComponent(USceneComponent* Child);
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_SetCollisionEnabled(UPrimitiveComponent* Object, ECollisionEnabled::Type Value);
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_SetHiddenInGame(USceneComponent* Component, bool NewHidden);
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_AddAngularImpulseInDegrees(UPrimitiveComponent* Component, FVector Impulse, FName BoneName = NAME_None, bool bVelChange = false);

private:
	// Client

};
