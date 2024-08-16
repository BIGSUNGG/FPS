// Fill out your copyright notice in the Description page of Project Settings.


#include "AdvanceMovementComponent.h"
#include Creature_h
#include CreatureAnimInstance_h

// Sets default values for this component's properties
UAdvanceMovementComponent::UAdvanceMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAdvanceMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UAdvanceMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UAdvanceMovementComponent, CurWallRunState, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UAdvanceMovementComponent, bIsSliding, COND_SkipOwner);
}

// Called every frame
void UAdvanceMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
	if (bWantToJump == true)
	{
		bWantToJump = false;
		OwnerCreature->Jump();
	}

	if (OwnerCreature->IsLocallyControlled())
	{
		WallRunUpdate();
		SlideUpdate();	
	}
}

void UAdvanceMovementComponent::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	ResetWallRunVerticalSuppression();
	ResetWallRunHorizonSuppression();
	ResetSlideSuppression();
	if (OwnerCreature->GetbCrouchButtonPress())
	{
		if (CanSlide())
			SlideStart();
		else
			OwnerCreature->Crouch();
	}

	bCanDoubleJump = true;
}

void UAdvanceMovementComponent::JumpStart()
{
	if (IsFalling() == false && IsWallRunning() == false) // 바닥에 있으면
	{
		if (bIsSliding) // 슬라이딩 중이면
		{
			SlideEnd(true);
			bWantToJump = true;
		}
		else if (IsCrouching()) // 앉아있으면
		{
			CrouchEnd();
			bWantToJump = true;
		}
		Super::JumpStart();
	}
	else if (IsWallRunning()) // 벽타기 중이면
		WallRunJump();
	else if (bCanDoubleJump) // 더블점프가 가능하면
		DoubleJump();
}

void UAdvanceMovementComponent::JumpEnd()
{
	Super::JumpEnd();
}

void UAdvanceMovementComponent::CrouchStart()
{
	if (IsFalling() == false && CanSlide()) // 슬라이딩이 가능하면
		SlideStart(); // 슬라이딩 시작
	else if (IsWallRunning()) // 벽타기 중이면
		WallRunEnd(0.3f); // 벽타기 중지
	else if (IsFalling() == false) // 지면에 있으면
		Super::CrouchStart(); // 앉기 시작
}

void UAdvanceMovementComponent::CrouchEnd()
{
	if (bIsSliding) // 슬라이딩 중이면
		SlideEnd(true); // 슬라이딩 중지
	else if (IsFalling() == false) // 지면에 있으면
		Super::CrouchEnd(); // 일어나기
}

void UAdvanceMovementComponent::WallRunJump()
{
	if (IsWallRunning()) // 벽타기 중이면
	{		
		// 벽타기 점프 방향 구하기
		FVector LaunchVector;
		LaunchVector.X = WallRunNormal.X * WallRunJumpOffForce;
		LaunchVector.Y = WallRunNormal.Y * WallRunJumpOffForce;
		LaunchVector.Z = WallRunJumpHeight;
		OwnerCreature->LaunchCharacter(LaunchVector, false, true);
		OwnerCreature->OnJumped();
		KR_LOG_VECTOR(WallRunNormal);
		KR_LOG_VECTOR(LaunchVector);
		KR_LOG_VECTOR(PendingLaunchVelocity);

		Server_WallRunJump(Velocity, PendingLaunchVelocity);
		WallRunEnd(0.35); // 벽타기 중지
	}
	else
	{
		KR_LOG(Warning, TEXT("IsWallRunning is false"))
	}
}

void UAdvanceMovementComponent::DoubleJump()
{
	bCanDoubleJump = false;

	FVector LaunchPower(0, 0, DobuleJumpPower.Z);

	UCharacterMovementComponent* MovementComp = OwnerCreature->GetCharacterMovement();

	// 현재 속도 구하기
	FVector Right = OwnerCreature->GetControlRotation().Vector().GetSafeNormal2D().RotateAngleAxis(-90.0f, FVector::UpVector);
	float RightSpeed = -FVector::DotProduct(Velocity, Right) / Right.Size2D();

	FVector Forward = OwnerCreature->GetControlRotation().Vector().GetSafeNormal2D();
	float ForwardSpeed = FVector::DotProduct(Velocity, Forward) / Forward.Size2D();

	// 돌진 속도 구하기
	float ForwardLaunchPower;
	float RightLaunchPower;

	// 현재 정면 속도가 돌진 속도보다 빠르다면
	if ((CurrentInputForward > 0.f && ForwardSpeed > DobuleJumpPower.X * CurrentInputForward) ||
		(CurrentInputForward < 0.f && -ForwardSpeed > DobuleJumpPower.X * -CurrentInputForward)) 
	{
		ForwardLaunchPower = ForwardSpeed;
	}
	// 현재 정면 속도보다 돌진 속도보다 빠르다면
	else
	{
		ForwardLaunchPower = DobuleJumpPower.X * CurrentInputForward;
	}

	// 현재 좌우 속도가 돌진 속도보다 빠르다면
	if ((CurrentInputRight > 0.f && RightSpeed > DobuleJumpPower.Y * CurrentInputRight) ||
		(CurrentInputRight < 0.f && -RightSpeed > DobuleJumpPower.Y * -CurrentInputRight))
	{
		RightLaunchPower = RightSpeed;
	}
	// 현재 좌우 속도보다 돌진 속도보다 빠르다면
	else
	{
		RightLaunchPower = DobuleJumpPower.Y * CurrentInputRight;
	}

	// 돌진 실행
	bool bOverideXY = false;
	if (CurrentInputForward != 0.f)
	{
		FRotator Rotation = OwnerCreature->GetController()->GetControlRotation();
		FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		LaunchPower += FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) * ForwardLaunchPower;
		bOverideXY = true;
	}
	if (CurrentInputRight != 0.f)
	{
		FRotator Rotation = OwnerCreature->GetController()->GetControlRotation();
		FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		LaunchPower += FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) * RightLaunchPower;
		bOverideXY = true;
	}

	OwnerCreature->LaunchCharacter(LaunchPower, bOverideXY, true);
	OwnerCreature->OnJumped();

	Server_DoubleJump(LaunchPower);

	KR_LOG(Log, TEXT("Dobule Jump Power : %f %f"), ForwardLaunchPower, RightLaunchPower);
}

bool UAdvanceMovementComponent::WallRunUpdate()
{
	if(CurWallRunState != EWallRunState::WALLRUN_VERTICAL) // 세로 벽타기 중이면
		CurWallRunVerticalSpeed = FMath::FInterpTo(CurWallRunVerticalSpeed, MaxWallRunVerticalSpeed, GetWorld()->GetDeltaSeconds(), 5.f); // 세로 벽타기 속도 줄이기

	if (OwnerCreature->GetbJumpButtonPress() || IsWallRunning()) // 점프 버튼을 누르고있거나 벽타기 중이면
	{
		bool WallRunVerticalSuccess = false;
		bool WallRunHorizonSuccess = false;

		if (!bWallRunVerticalSupressed) // 세로 벽타기가 가능한지
		{
			WallRunVerticalSuccess = WallRunVerticalUpdate(); // 세로 벽타기 시작
			if (WallRunVerticalSuccess) // 벽타기 성공했을때
			{
				FVector WallRunDir = PendingLaunchVelocity;
				WallRunDir.Normalize();
				Server_WallRunVerticalUpdate(WallRunDir, CurWallRunVerticalSpeed);
				return true;
			}
		}

		if (!bWallRunHorizonSupressed) // 가로 벽타기가 가능한지
		{
			WallRunHorizonSuccess = WallRunHorizonUpdate(); // 가로 벽타기 시작
			if (WallRunHorizonSuccess) // 벽타기 성공했을때
			{
				FVector WallRunDir = PendingLaunchVelocity;
				WallRunDir.Normalize();
				Server_WallRunHorizonUpdate(WallRunDir);
				return true;
			}
		}
	}

	return false;
}

bool UAdvanceMovementComponent::WallRunHorizonUpdate()
{
	if ((CurWallRunState == EWallRunState::NONE || CurWallRunState == EWallRunState::WALLRUN_RIGHT) && WallRunHorizonMovement(OwnerCreature->GetActorLocation(), CalculateRightWallRunEndVector(), -1.f))
		// 오른쪽 벽타기 확인
	{
		if (!IsWallRunning())
			WallRunStart(EWallRunState::WALLRUN_RIGHT);

		return true;
	}
	else if (CurWallRunState == EWallRunState::WALLRUN_RIGHT)
		// 오른쪽 벽타기 중이였지만 오른쪽 벽타기를 실패한경우
	{
		WallRunHorizonEnd(1.f);
		return false;
	}
	else if ((CurWallRunState == EWallRunState::NONE || CurWallRunState == EWallRunState::WALLRUN_LEFT) && WallRunHorizonMovement(OwnerCreature->GetActorLocation(), CalculateLeftWallRunEndVector(), 1.f))
		// 왼쪽 벽타기 확인
	{
		if (!IsWallRunning())
			WallRunStart(EWallRunState::WALLRUN_LEFT);

		return true;
	}
	else if (CurWallRunState != EWallRunState::WALLRUN_LEFT)
		// 왼쪽 벽타기 중이였지만 왼쪽 벽타기를 실패한경우
	{
		WallRunHorizonEnd(1.f);
		return false;
	}

	return false;
}

bool UAdvanceMovementComponent::WallRunVerticalUpdate()
{
	if (bWallRunVerticalSupressed) // 세로 벽타기가 가능한지
		return false;

	if (CurWallRunVerticalSpeed <= 100.f) // 세로 벽타기 속도가 부족하면
	{
		WallRunVerticalEnd(2.f); // 세로 벽타기 중지
		return false;
	}

	if (WallRunVerticalMovement(OwnerCreature->GetActorLocation(), CalculateVerticaltWallRunEndVector()))
		// 세로 벽타기가 성공했는지
	{
		if (!IsWallRunning())
			WallRunStart(EWallRunState::WALLRUN_VERTICAL);

		CurWallRunVerticalSpeed = FMath::FInterpTo(CurWallRunVerticalSpeed, 0.f, GetWorld()->GetDeltaSeconds(), 0.75f);
		return true;
	}
	else
	{
		WallRunVerticalEnd(1.f);
		return false;
	}
}

bool UAdvanceMovementComponent::WallRunHorizonMovement(FVector Start, FVector End, float WallRunDirection)
{
	if (bWallRunHorizonSupressed)
		return false;

	// 벽타기할 오브젝트 찾기
	FCollisionQueryParams ObjectParams(NAME_None, false, OwnerCreature);
	FHitResult Result;
	bool bSuccess = GetWorld()->LineTraceSingleByChannel(
		Result,
		Start,
		End,
		ECC_ONLY_OBJECT,
		ObjectParams
	);

	if (bSuccess && Result.bBlockingHit)
	{
		if ((IsFalling() || IsWallRunning()) && CalculateValidWallVector(Result.Normal))
		{
			// 벽 각도에 맞춰 이동
			WallRunNormal = Result.Normal;
			FVector WallRunVec = FVector::CrossProduct(WallRunNormal, FVector(0, 0, 1)) * (WallRunDirection * WallRunHorizonSpeed);
			OwnerCreature->LaunchCharacter(WallRunVec, true, !bWallRunGravity);

			FRotator Temp = WallRunVec.Rotation();
			return true;
		}
	}
	return false;
}

void UAdvanceMovementComponent::SlideStart()
{
	if (!CanSlide())
		return;

	UCharacterMovementComponent* MovementComp = OwnerCreature->GetCharacterMovement();

	float RightSpeed = OwnerCreature->CalculateRightSpeed();

	bIsSliding = true;

	FVector SlidePower;
	
	{
		FRotator Rotation = OwnerCreature->GetController()->GetControlRotation();
		FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		SlidePower += FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) * SlideSpeed;
	}

	{
		FRotator Rotation = OwnerCreature->GetController()->GetControlRotation();
		FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		SlidePower += FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) * RightSpeed;
	}

	InputForwardRatio = 0.4f;
	InputRightRatio = 0.4f;
	GroundFriction = SlideGroundFriction;
	BrakingDecelerationWalking = SlideBrakingDecelerationWalking;
	Velocity.X = SlidePower.X;
	Velocity.Y = SlidePower.Y;
	Super::CrouchStart();

	Server_SlideStart(Velocity);

	KR_LOG(Log, TEXT("Slide Start : %f %f"), SlidePower.X, SlidePower.Y);
}

void UAdvanceMovementComponent::SlideEnd(bool DoUncrouch)
{
	if (!bIsSliding || bSlideSupressed)
		return;

	bIsSliding = false;

	// Movement 설정 되돌리기
	InputForwardRatio = 1.f;
	InputRightRatio = 1.f;
	GroundFriction = DefaultGroundFriction;
	BrakingDecelerationWalking = DefaultBrakingDecelerationWalking;
	
	// 일어나기
	if (DoUncrouch)
		OwnerCreature->UnCrouch(true);

	SuppressSlide(0.5f);
	Server_SlideEnd();
	KR_LOG(Log, TEXT("Slide End"));
}

void UAdvanceMovementComponent::SuppressSlide(float Delay)
{
	bSlideSupressed = true;
	OwnerCreature->GetWorldTimerManager().SetTimer(SuppressSlideTimer, this, &UAdvanceMovementComponent::ResetSlideSuppression, Delay, false);
}

void UAdvanceMovementComponent::ResetSlideSuppression()
{
	bSlideSupressed = false;
	OwnerCreature->GetWorldTimerManager().ClearTimer(SuppressSlideTimer);
}

void UAdvanceMovementComponent::Server_WallRunStart_Implementation(EWallRunState State)
{
	GravityScale = 0.f;
	PendingLaunchVelocity.Z = 0.f;
	bCanDoubleJump = true;
	CurWallRunState = State;
}

bool UAdvanceMovementComponent::WallRunVerticalMovement(FVector Start, FVector End)
{
	FCollisionQueryParams ObjectParams(NAME_None, false, OwnerCreature);
	FHitResult Result;
	bool bSuccess = GetWorld()->LineTraceSingleByChannel(
		Result,
		Start,
		End,
		ECC_ONLY_OBJECT,
		ObjectParams
	);

	if (bSuccess && Result.bBlockingHit)
	{
		if ((IsFalling() || IsWallRunning()) && CalculateValidWallVector(Result.Normal))
		{
			WallRunNormal = Result.Normal;

			OwnerCreature->LaunchCharacter(OwnerCreature->GetActorUpVector() * CurWallRunVerticalSpeed, true, true);
			return true;
		}
	}
	return false;
}

void UAdvanceMovementComponent::WallRunStart(EWallRunState State)
{
	if (IsWallRunning())
		return;

	if (bWallRunGravity) // 벽타기 중 개별적인 중력을 적용할지
		GravityScale = WallRunTargetGravity; // 중력 적용

	PendingLaunchVelocity.Z = 0.f;
	bCanDoubleJump = true;
	CurWallRunState = State;
	Server_WallRunStart(State);
}

void UAdvanceMovementComponent::WallRunEnd(float ResetTime)
{
	if (IsWallRunning() == false)
		return;

	CurWallRunState = EWallRunState::NONE;
	GravityScale = DefaultGravity;
	SuppressWallRunHorizion(ResetTime);
	SuppressWallRunVertical(ResetTime);
	Server_WallRunEnd();
}

void UAdvanceMovementComponent::WallRunHorizonEnd(float ResetTime)
{
	if (IsWallRunning() == false)
		return;

	CurWallRunState = EWallRunState::NONE;
	GravityScale = DefaultGravity;
	SuppressWallRunHorizion(ResetTime);
	Server_WallRunEnd();
}

void UAdvanceMovementComponent::WallRunVerticalEnd(float ResetTime)
{
	if (IsWallRunning() == false)
		return;

	CurWallRunState = EWallRunState::NONE;
	GravityScale = DefaultGravity;
	SuppressWallRunVertical(ResetTime);
	Server_WallRunEnd();
}

void UAdvanceMovementComponent::ResetWallRunHorizonSuppression()
{
	bWallRunHorizonSupressed = false;
	OwnerCreature->GetWorldTimerManager().ClearTimer(SuppressWallRunHorizonTimer);
}

void UAdvanceMovementComponent::ResetWallRunVerticalSuppression()
{
	bWallRunVerticalSupressed = false;
	OwnerCreature->GetWorldTimerManager().ClearTimer(SuppressWallRunVerticalTimer);
}

void UAdvanceMovementComponent::SuppressWallRunHorizion(float Delay)
{
	bWallRunHorizonSupressed = true;
	OwnerCreature->GetWorldTimerManager().SetTimer(SuppressWallRunHorizonTimer, this, &UAdvanceMovementComponent::ResetWallRunHorizonSuppression, Delay, false);
}

void UAdvanceMovementComponent::SuppressWallRunVertical(float Delay)
{
	bWallRunVerticalSupressed = true;
	OwnerCreature->GetWorldTimerManager().SetTimer(SuppressWallRunVerticalTimer, this, &UAdvanceMovementComponent::ResetWallRunVerticalSuppression, Delay, false);
}

FVector UAdvanceMovementComponent::CalculateRightWallRunEndVector()
{
	return (
		OwnerCreature->GetActorLocation() +
		OwnerCreature->GetActorRightVector() * 75.f +
		OwnerCreature->GetActorForwardVector() * -75.f
		);
}

FVector UAdvanceMovementComponent::CalculateLeftWallRunEndVector()
{
	return (
		OwnerCreature->GetActorLocation() +
		OwnerCreature->GetActorRightVector() * -75.f +
		OwnerCreature->GetActorForwardVector() * -75.f
		);
}

FVector UAdvanceMovementComponent::CalculateVerticaltWallRunEndVector()
{
	return (
		OwnerCreature->GetActorLocation() +
		OwnerCreature->GetActorForwardVector() * 75.f
		);
}

bool UAdvanceMovementComponent::CalculateValidWallVector(FVector InVec)
{
	return UKismetMathLibrary::InRange_FloatFloat(InVec.Z, -0.52f, 0.52, false, false);
}

bool UAdvanceMovementComponent::CanSlide()
{
	if (bSlideSupressed)
		return false;

	if (bIsSliding)
		return false;

	if (WalkState != EMovementState::SPRINT)
		return false;

	if (FVector2D(Velocity.X, Velocity.Y).Size() < MinSlideRequireSpeed)
		return false;

	return true;
}

void UAdvanceMovementComponent::SlideUpdate()
{
	if (!bIsSliding)
		return;

	float Speed = FVector2D(Velocity.X, Velocity.Y).Size();
	if (Speed < CrouchWalkSpeed)
	{
		SlideEnd(false);
		return;
	}

	// 미끄러질 힘 구하기
	FVector Slope = OwnerCreature->CaclulateCurrentFloorSlopeVector();
	FVector SlopePhysics = Slope * SlideSlopeSpeed;
	SlopePhysics.Z = 0.f;
	Velocity += SlopePhysics;

	Server_SlideUpdate(OwnerCreature->GetActorLocation(), Velocity);
}

void UAdvanceMovementComponent::Server_WallRunJump_Implementation(FVector InVelocity, FVector InPendingLaunchVelocity)
{
	Velocity = InVelocity;
	PendingLaunchVelocity = InPendingLaunchVelocity;

	Multicast_WallRunJump();
}

bool UAdvanceMovementComponent::Server_WallRunJump_Validate(FVector InVelocity, FVector InPendingLaunchVelocity)
{
	if (CurWallRunState == EWallRunState::NONE)
		return false;

	return true;
}

void UAdvanceMovementComponent::Multicast_WallRunJump_Implementation()
{
	UCreatureAnimInstance* CreatureAnim = Cast<UCreatureAnimInstance>(OwnerCreature->GetMesh()->GetAnimInstance());
	OwnerCreature->GetMesh()->GetAnimInstance()->Montage_Play(CreatureAnim->GetJumpMontage());
}

void UAdvanceMovementComponent::Server_WallRunEnd_Implementation()
{
	CurWallRunState = EWallRunState::NONE;
	GravityScale = DefaultGravity;
}

bool UAdvanceMovementComponent::Server_WallRunEnd_Validate()
{
	if (CurWallRunState == EWallRunState::NONE)
		return false;

	return true;
}

void UAdvanceMovementComponent::Server_DoubleJump_Implementation(FVector LaunchVelocity)
{
	bCanDoubleJump = false;
	PendingLaunchVelocity = LaunchVelocity;
	Multicast_DoubleJump(LaunchVelocity);
}

bool UAdvanceMovementComponent::Server_DoubleJump_Validate(FVector LaunchInVelocity)
{
	if (bCanDoubleJump == false)
		return false;

	return true;
}

void UAdvanceMovementComponent::Multicast_DoubleJump_Implementation(FVector LaunchVelocity)
{
	UCreatureAnimInstance* CreatureAnim = Cast<UCreatureAnimInstance>(OwnerCreature->GetMesh()->GetAnimInstance());
	OwnerCreature->GetMesh()->GetAnimInstance()->Montage_Play(CreatureAnim->GetJumpMontage());
}

void UAdvanceMovementComponent::Server_WallRunHorizonUpdate_Implementation(FVector Direction)
{
	PendingLaunchVelocity = Direction * WallRunHorizonSpeed;
}

bool UAdvanceMovementComponent::Server_WallRunHorizonUpdate_Validate(FVector Direction)
{
	if (CurWallRunState == EWallRunState::WALLRUN_LEFT)
		return true;

	if (CurWallRunState == EWallRunState::WALLRUN_RIGHT)
		return true;

	return false;
}

void UAdvanceMovementComponent::Server_WallRunVerticalUpdate_Implementation(FVector Direction, float WallRunVerticalSpeed)
{
	PendingLaunchVelocity = Direction * WallRunVerticalSpeed;
}	

bool UAdvanceMovementComponent::Server_WallRunVerticalUpdate_Validate(FVector Direction, float WallRunVerticalSpeed)
{
	if (CurWallRunState != EWallRunState::WALLRUN_VERTICAL)
		return false;

	return true;
}

void UAdvanceMovementComponent::Server_SlideStart_Implementation(FVector InVelocity)
{
	bIsSliding = true;

	GroundFriction = SlideGroundFriction;
	BrakingDecelerationWalking = SlideBrakingDecelerationWalking;
	Velocity = InVelocity;
}

void UAdvanceMovementComponent::Server_SlideEnd_Implementation()
{
	bIsSliding = false;

	GroundFriction = DefaultGroundFriction;
	BrakingDecelerationWalking = DefaultBrakingDecelerationWalking;
}

bool UAdvanceMovementComponent::Server_SlideEnd_Validate()
{
	if (bIsSliding == false)
		return false;

	return true;
}

void UAdvanceMovementComponent::Server_SlideUpdate_Implementation(FVector Location, FVector InVelocity)
{
	OwnerCreature->SetActorLocation(Location);
	Velocity = InVelocity;
}

bool UAdvanceMovementComponent::Server_SlideUpdate_Validate(FVector Location, FVector InVelocity)
{
	if (bIsSliding == false)
		return false;

	return true;
}

bool UAdvanceMovementComponent::IsFalling() const
{
	if (IsWallRunning())
		return false;

	return Super::IsFalling();
}
