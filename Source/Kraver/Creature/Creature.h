// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Character.h"
#include "Creature.generated.h"

UCLASS()
class KRAVER_API ACreature : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACreature();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	FORCEINLINE FRotator GetCreatureAngle() { return Camera->GetComponentRotation() - GetMesh()->GetComponentRotation(); }

	bool GetIsRunning() {return IsRunning;}

protected:
	void MoveForward(float NewAxisValue);
	void MoveRight(float NewAxisValue);
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);

	void RunButtonPressed();
	void CrouchButtonPressed();
	void JumpingButtonPressed();
	void JumpingButtonReleased();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CAMERA, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CAMERA, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		bool IsJumping = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		bool IsRunning = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		float RunSpeed = 600.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		float WalkSpeed = 350.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = CREATURE, meta = (AllowPrivateAccess = "true"))
		float CrouchSpeed = 200.f;
}; 
