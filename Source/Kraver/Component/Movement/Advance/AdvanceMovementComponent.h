// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include CreatureMovementComponent_h
#include "Components/ActorComponent.h"
#include "AdvanceMovementComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAVER_API UAdvanceMovementComponent : public UCreatureMovementComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAdvanceMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void Landed(const FHitResult& Hit);

	virtual void JumpStart() override;
	virtual void JumpEnd() override;

	virtual void Crouch() override;
	virtual void UnCrouch() override;

protected:
	// Double Jump
	void DoubleJump(); // 더블점프

	// Wall Run
	void WallRunJump(); // 벽타기 점프
	bool WallRunUpdate(); // 벽타기중 Tick마다 호출
	bool WallRunHorizonUpdate(); // 가로 벽타기중 Tick마다 호출
	bool WallRunVerticalUpdate(); // 세로 벽타기중 Tick마다 호출
	bool WallRunHorizonMovement(FVector Start, FVector End, float WallRunDirection); // 가로 벽타기 이동
	bool WallRunVerticalMovement(FVector Start, FVector End);  // 세로 벽타기 이동
	void WallRunStart(EWallRunState State); // 벽타기 시작 (가로 세로 포함)
	void WallRunEnd(float ResetTime); // 벽타기 종료 (가로 세로 포함)
	void WallRunHorizonEnd(float ResetTime); // 가로 벽타기 종료
	void WallRunVerticalEnd(float ResetTime); // 세로 벽타기 종료
	void ResetWallRunHorizonSuppression(); // 가로 벽타기 가능하게 초기화
	void ResetWallRunVerticalSuppression(); // 세로 벽타기 가능하게 초기화
	void SuppressWallRunHorizion(float Delay);
	void SuppressWallRunVertical(float Delay);

	FVector CalculateRightWallRunEndVector();
	FVector CalculateLeftWallRunEndVector();
	FVector CalculateVerticaltWallRunEndVector();
	bool CalculateValidWallVector(FVector InVec);

	// Slide
	bool CanSlide(); // 슬라이딩 시작이 가능한지
	void SlideUpdate(); // 슬라이딩중 Tick마다 호출
	void SlideStart(); // 슬라이딩 시작시 호출
	void SlideEnd(bool DoUncrouch); // 슬라이딩 종료시 호출
	void SuppressSlide(float Delay);
	void ResetSlideSuppression();

	// Rpc
	UFUNCTION(Server, Reliable)
	virtual void Server_WallRunStart(EWallRunState State);
	UFUNCTION(Server, Reliable)
	virtual void Server_WallRunJumpSuccess(FVector PendingLaunchVelocity);
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_WallRunJumpSuccess(FVector PendingLaunchVelocity);
	UFUNCTION(Server, Reliable)
	virtual void Server_WallRunEnd();

	UFUNCTION(Server, Reliable)
	virtual void Server_DoubleJump(FVector PendingLaunchVelocity);
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_DoubleJump(FVector PendingLaunchVelocity);

	UFUNCTION(Server, Reliable)
	virtual void Server_WallRunHorizonSuccess();
	UFUNCTION(Server, Reliable)
	virtual void Server_WallRunVerticalSuccess();
	UFUNCTION(Server, Reliable)
	virtual void Server_WallRunSuccess(FVector Location, FRotator Rotation, FVector Velocity, FVector PendingLaunchVelocity);

	UFUNCTION(Server, Reliable)
	virtual void Server_SlideSuccess(FVector Velocity);
	UFUNCTION(Server, Reliable)
	virtual void Server_SlideEnd();
	UFUNCTION(Server, Reliable)
	virtual void Server_SlideUpdate(FVector Location, FVector Velocity);

public:
	// Getter Setter
	virtual bool IsFalling() override;
	bool IsSliding() { return bIsSliding; }
	const bool IsWallRunning() { return CurWallRunState != EWallRunState::NONE; }

	EWallRunState GetCurWallRunState() { return CurWallRunState; }

protected:	
	// Movement
	bool bWantToJump = false;

	// Advanced Movement / Double Jump
	bool bCanDoubleJump = true; // 더블점프가 가능한지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Movement|AdvancedMovement", meta = (AllowPrivateAccess = "true"))
	FVector DobuleJumpPower = FVector(600, 600, 450); // 더블점프 힘

	// Advanced Movement / Wall Run
	UPROPERTY(Replicated)
	EWallRunState CurWallRunState; // 현재 무슨 벽타기 중인지

	bool bWallRunHorizonSupressed = false; // 가로 벽타기가 불가능한지
	bool bWallRunVerticalSupressed = false; // 세로 벽타기가 불가능한지
	bool bWallRunGravity = true; // 벽타기 중 중력을 따로 정할지
	float WallRunHorizonSpeed = 900.f; // 가로 벽타기 속도
	float MaxWallRunVerticalSpeed = 600.f; // 세로 벽타기 최대 속도
	float CurWallRunVerticalSpeed = 600.f; // 현재 세로 벽타기 속도
	float WallRunJumpHeight = 400.f; // 벽타기 도중 점프 높이
	float WallRunJumpOffForce  = 400.f; 
	float WallRunTargetGravity = 0.75f; // 벽타기 중 받을 중력
	FVector WallRunNormal; // 현재 벽타는 중인 벽의 각도
	FTimerHandle SuppressWallRunHorizonTimer;
	FTimerHandle SuppressWallRunVerticalTimer;

	// Advanced Movement / Slide
	UPROPERTY(Replicated)
	bool bIsSliding = false; // 슬라이딩 중인지

	bool bSlideSupressed = false; // 슬라이딩이 불가능한지
	float MinSlideRequireSpeed = 600.f; // 슬라이딩을 시작하기 위해 필요한 최소 속도
	float SlideSlopeSpeed = 20.f; // 바닥 각도에 따라 미끄러지는 속도
	float SlideSpeed = 1500.f; // 슬라이딩 시작할때 속도
	float SlideGroundFriction = 0.f;
	float SlideBrakingDecelerationWalking = 1400.f;
	FTimerHandle SuppressSlideTimer;

};
