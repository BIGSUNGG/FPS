// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "Components/ActorComponent.h"
#include "ServerComponent.generated.h"


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
	void AttachComponentToComponent(USceneComponent* Child, USceneComponent* Parent);
	void DetachComponentFromComponent(USceneComponent* Child);
	void SetPhysicsLinearVelocity(UPrimitiveComponent* Component, FVector Velocity);
	void AddImpulse(UPrimitiveComponent* Component, FVector Direction, FName BoneName = NAME_None, bool bVelChange = false);
	void SetLocation(UPrimitiveComponent* Component, FVector Location);
	void SetRotation(UPrimitiveComponent* Component, FRotator Location);

private:
	UFUNCTION(Server, Reliable)
		void Server_OwningOtherActor(AActor* Actor);
	UFUNCTION(Server, reliable)
		void Server_SetSimulatedPhysics(UPrimitiveComponent* Component, bool bSimulated);
	UFUNCTION(Server, Reliable)
		void Server_AttachComponentToComponent(USceneComponent* Child, USceneComponent* Parent);
	UFUNCTION(Server, Reliable)
		void Server_DetachComponentFromComponent(USceneComponent* Child);
	UFUNCTION(Server, Reliable)
		void Server_SetPhysicsLinearVelocity(UPrimitiveComponent* Component, FVector Velocity);
	UFUNCTION(Server, Reliable)
		void Server_AddImpulse(UPrimitiveComponent* Component, FVector Direction, FName BoneName = NAME_None, bool bVelChange = false);
	UFUNCTION(Server, Reliable)
		void Server_SetLocation(UPrimitiveComponent* Component, FVector Location);
	UFUNCTION(Server, Reliable)
		void Server_SetRotation(UPrimitiveComponent* Component, FRotator Rotation);
};
