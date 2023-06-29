// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Component/Movement/CreatureMovementComponent.h"
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
	void DoubleJump();

	// Wall Run
	void WallRunJump();
	bool WallRunUpdate();
	bool WallRunHorizonUpdate();
	bool WallRunVerticalUpdate();
	bool WallRunHorizonMovement(FVector Start, FVector End, float WallRunDirection);
	bool WallRunVerticalMovement(FVector Start, FVector End);
	void WallRunStart();
	void WallRunEnd(float ResetTime);
	void WallRunHorizonEnd(float ResetTime);
	void WallRunVerticalEnd(float ResetTime);
	void ResetWallRunHorizonSuppression();
	void ResetWallRunVerticalSuppression();
	void SuppressWallRunHorizion(float Delay);
	void SuppressWallRunVertical(float Delay);
	FVector CalculateRightWallRunEndVector();
	FVector CalculateLeftWallRunEndVector();
	FVector CalculateVerticaltWallRunEndVector();
	bool CalculateValidWallVector(FVector InVec);
	FVector CalculatePlayerToWallVector();

	// Slide
	bool CanSlide();
	void SlideUpdate();
	void SlideStart();
	void SlideEnd(bool DoUncrouch);
	void SuppressSlide(float Delay);
	void ResetSlideSuppression();

	// Rpc
	UFUNCTION(Server, reliable)
		virtual void Server_WallRunJumpSuccess(FVector PendingLaunchVelocity);
	UFUNCTION(NetMulticast, reliable)
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
		virtual void Server_SlideUpdate(FVector Velocity);

public:
	// Getter Setter
	EWallRunState GetCurWallRunState() { return CurWallRunState; }
	bool GetIsSliding() { return IsSliding; }
	const bool GetIsWallRunning() { return CurWallRunState != EWallRunState::NONE; }

protected:	
	// Movement
	bool bWantToJump = false;

	// Advanced Movement / Double Jump
	bool bCanDoubleJump = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Movement|AdvancedMovement", meta = (AllowPrivateAccess = "true"))
		FVector DobuleJumpPower = FVector(600, 600, 450);

	// Advanced Movement / Wall Run
	UPROPERTY(Replicated)
		EWallRunState CurWallRunState;
	void SetCurWallRunState(EWallRunState Value);
	UFUNCTION(Server, reliable)
		void Server_SetCurWallRunState(EWallRunState Value);

	bool bWallRunHorizonSupressed = false;
	bool bWallRunVerticalSupressed = false;
	bool bWallRunGravity = true;
	float WallRunHorizonSpeed = 1200.f;
	float WallRunVerticalSpeed = 600.f;
	float WallRunJumpHeight = 400.f;
	float WallRunJumpOffForce  = 400.f;
	float WallRunTargetGravity = 0.75f;
	FVector WallRunNormal;
	FTimerHandle SuppressWallRunHorizonTimer;
	FTimerHandle SuppressWallRunVerticalTimer;

	// Advanced Movement / Slide
	UPROPERTY(Replicated)
		bool IsSliding = false;
	void SetIsSliding(bool Value);
	UFUNCTION(Server, Reliable)
		void Server_SetIsSliding(bool Value);

	bool bSlideSupressed = false;
	float MinSlideRequireSpeed = 900.f;
	float SlideSlopeSpeed = 40.f;
	float SlideSpeed = 2000.f;
	float SlideDuration = 2.f;
	float SlideGroundFriction = 0.f;
	float SlideBrakingDecelerationWalking = 1400.f;
	FTimerHandle SuppressSlideTimer;
};
