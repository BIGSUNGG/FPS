// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "Components/ActorComponent.h"
#include "ServerComponent.generated.h"

// 서버에서 호출되는 함수를 가지는 컴포넌트
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAVER_API UServerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UServerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void OwningOtherActor(AActor* Actor);
	void SetSimulatedPhysics(UPrimitiveComponent* Component, bool bSimulated);
	void AttachComponentToComponent(USceneComponent* Child, USceneComponent* Parent, FName BoneName = NAME_None);
	void DetachComponentFromComponent(USceneComponent* Child);
	void SetPhysicsLinearVelocity(UPrimitiveComponent* Component, FVector Velocity);
	void AddImpulse(UPrimitiveComponent* Component, FVector Direction, FName BoneName = NAME_None, bool bVelChange = false);
	void AddImpulseAtLocation(UPrimitiveComponent* Component, FVector Direction, FVector Location, FName BoneName = NAME_None);
	void SetLocation(UPrimitiveComponent* Component, FVector Location);
	void SetRotation(UPrimitiveComponent* Component, FRotator Location);
	void SetCharacterWalkSpeed(ACharacter* Character, float Speed);
	void PlayMontage(USkeletalMeshComponent* Mesh, UAnimMontage* Montage, float Speed = 1.f);
	void SetCollisionProfileName(UPrimitiveComponent* Component, FName ProfileName);

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
		void Server_SetLocation(UPrimitiveComponent* Component, FVector Location);
	UFUNCTION(Server, Reliable)
		void Server_SetRotation(UPrimitiveComponent* Component, FRotator Rotation);
	UFUNCTION(Server, Reliable)
		void Server_SetCharacterWalkSpeed(ACharacter* Character, float Speed);
	UFUNCTION(Server, Reliable)
		void Server_PlayMontage(USkeletalMeshComponent* Mesh, UAnimMontage* Montage, float Speed = 1.f);
	UFUNCTION(Server, Reliable)
		void Server_SetCollisionProfileName(UPrimitiveComponent* Component, FName ProfileName);

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
		void Multicast_PlayMontage(USkeletalMeshComponent* Mesh, UAnimMontage* Montage, float Speed = 1.f);

private:
	// Client

};
