// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CreatureMovementComponent.generated.h"

// Creature의 이동을 담당하는 컴포넌트
UCLASS(Config = Game)
class KRAVER_API UCreatureMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCreatureMovementComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void Landed(const FHitResult& Hit);

	virtual void MoveForward(float NewAxisValue); // 정면으로 이동
	virtual void MoveRight(float NewAxisValue); // 측면으로 이동

	virtual void JumpStart(); // 점프 시작
	virtual void JumpEnd(); // 점프 종료

	virtual void CrouchStart(); // 앉기 
	virtual void CrouchEnd(); // 일어나기

protected:	
	UFUNCTION(Server, Reliable)
	void Server_SetMovementState(EMovementState value);

public:
	// Getter Setter
	FORCEINLINE bool IsJumping() { return bIsJumping; }
	FORCEINLINE float GetSprintSpeed() { return SprintSpeed; }
	FORCEINLINE float GetWalkSpeed() { return WalkSpeed; }
	FORCEINLINE float GetCrouchSprintSpeed() { return CrouchSprintSpeed; }
	FORCEINLINE float GetCrouchRunSpeed() { return CrouchRunSpeed; }
	FORCEINLINE float GetCrouchWalkSpeed() { return CrouchWalkSpeed; }
	FORCEINLINE EMovementState GetMovementState() { return WalkState; }

	virtual void SetMovementState(EMovementState value) final;

protected:
	class ACreature* OwnerCreature;
	float InputForwardRatio = 1.f;
	float InputRightRatio = 1.f;

	UPROPERTY(Replicated , VisibleAnywhere, BlueprintReadOnly, Category = "Data|Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsJumping = false;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Data|State", meta = (AllowPrivateAccess = "true"))
	EMovementState WalkState = EMovementState::WALK;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data|Movement", meta = (AllowPrivateAccess = "true"))
	float SprintSpeed = 900.f; // EMovementState가 SPRINT가 되었을때 설정할 캐릭터 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data|Movement", meta = (AllowPrivateAccess = "true"))
	float RunSpeed = 400.f; // EMovementState가 RUN가 되었을때 설정할 캐릭터 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data|Movement", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed = 400.f; // EMovementState가 WALK가 되었을때 설정할 캐릭터 속도

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data|Movement", meta = (AllowPrivateAccess = "true"))
	float CrouchWalkSpeed = 200.f; // 앉았을때 설정할 캐릭터 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data|Movement", meta = (AllowPrivateAccess = "true"))
	float CrouchRunSpeed = 200.f; // 앉았을때 설정할 캐릭터 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data|Movement", meta = (AllowPrivateAccess = "true"))
	float CrouchSprintSpeed = 450.f; // 앉았을때 설정할 캐릭터 속도

	// Default Movement
	float DefaultGravity = 0.f;
	float DefaultGroundFriction = 0.f;
	float DefaultBrakingDecelerationWalking = 0.f;

};
