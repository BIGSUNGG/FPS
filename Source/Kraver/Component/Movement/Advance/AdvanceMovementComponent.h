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
	void DoubleJump(); // ��������

	// Wall Run
	void WallRunJump(); // ��Ÿ�� ����
	bool WallRunUpdate(); // ��Ÿ���� Tick���� ȣ��
	bool WallRunHorizonUpdate(); // ���� ��Ÿ���� Tick���� ȣ��
	bool WallRunVerticalUpdate(); // ���� ��Ÿ���� Tick���� ȣ��
	bool WallRunHorizonMovement(FVector Start, FVector End, float WallRunDirection); // ���� ��Ÿ�� �̵�
	bool WallRunVerticalMovement(FVector Start, FVector End);  // ���� ��Ÿ�� �̵�
	void WallRunStart(EWallRunState State); // ��Ÿ�� ���� (���� ���� ����)
	void WallRunEnd(float ResetTime); // ��Ÿ�� ���� (���� ���� ����)
	void WallRunHorizonEnd(float ResetTime); // ���� ��Ÿ�� ����
	void WallRunVerticalEnd(float ResetTime); // ���� ��Ÿ�� ����
	void ResetWallRunHorizonSuppression(); // ���� ��Ÿ�� �����ϰ� �ʱ�ȭ
	void ResetWallRunVerticalSuppression(); // ���� ��Ÿ�� �����ϰ� �ʱ�ȭ
	void SuppressWallRunHorizion(float Delay);
	void SuppressWallRunVertical(float Delay);

	FVector CalculateRightWallRunEndVector();
	FVector CalculateLeftWallRunEndVector();
	FVector CalculateVerticaltWallRunEndVector();
	bool CalculateValidWallVector(FVector InVec);

	// Slide
	bool CanSlide(); // �����̵� ������ ��������
	void SlideUpdate(); // �����̵��� Tick���� ȣ��
	void SlideStart(); // �����̵� ���۽� ȣ��
	void SlideEnd(bool DoUncrouch); // �����̵� ����� ȣ��
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
	bool bCanDoubleJump = true; // ���������� ��������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Movement|AdvancedMovement", meta = (AllowPrivateAccess = "true"))
	FVector DobuleJumpPower = FVector(600, 600, 450); // �������� ��

	// Advanced Movement / Wall Run
	UPROPERTY(Replicated)
	EWallRunState CurWallRunState; // ���� ���� ��Ÿ�� ������

	bool bWallRunHorizonSupressed = false; // ���� ��Ÿ�Ⱑ �Ұ�������
	bool bWallRunVerticalSupressed = false; // ���� ��Ÿ�Ⱑ �Ұ�������
	bool bWallRunGravity = true; // ��Ÿ�� �� �߷��� ���� ������
	float WallRunHorizonSpeed = 900.f; // ���� ��Ÿ�� �ӵ�
	float MaxWallRunVerticalSpeed = 600.f; // ���� ��Ÿ�� �ִ� �ӵ�
	float CurWallRunVerticalSpeed = 600.f; // ���� ���� ��Ÿ�� �ӵ�
	float WallRunJumpHeight = 400.f; // ��Ÿ�� ���� ���� ����
	float WallRunJumpOffForce  = 400.f; 
	float WallRunTargetGravity = 0.75f; // ��Ÿ�� �� ���� �߷�
	FVector WallRunNormal; // ���� ��Ÿ�� ���� ���� ����
	FTimerHandle SuppressWallRunHorizonTimer;
	FTimerHandle SuppressWallRunVerticalTimer;

	// Advanced Movement / Slide
	UPROPERTY(Replicated)
	bool bIsSliding = false; // �����̵� ������

	bool bSlideSupressed = false; // �����̵��� �Ұ�������
	float MinSlideRequireSpeed = 600.f; // �����̵��� �����ϱ� ���� �ʿ��� �ּ� �ӵ�
	float SlideSlopeSpeed = 20.f; // �ٴ� ������ ���� �̲������� �ӵ�
	float SlideSpeed = 1500.f; // �����̵� �����Ҷ� �ӵ�
	float SlideGroundFriction = 0.f;
	float SlideBrakingDecelerationWalking = 1400.f;
	FTimerHandle SuppressSlideTimer;

};
