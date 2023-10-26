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

	WallRunUpdate();
	SlideUpdate();
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
	if (OwnerCreature->GetMovementComponent()->IsFalling() == false) // 공중에 있으면
	{
		if (bIsSliding) // 슬라이딩 중이면
		{
			SlideEnd(true);
			bWantToJump = true;
		}
		else if (OwnerCreature->GetCharacterMovement()->IsCrouching()) // 앉아있으면
		{
			UnCrouch();
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

void UAdvanceMovementComponent::Crouch()
{
	if (OwnerCreature->GetMovementComponent()->IsFalling() == false && CanSlide()) // 슬라이딩이 가능하면
		SlideStart(); // 슬라이딩 시작
	else if (IsWallRunning()) // 벽타기 중이면
		WallRunEnd(0.3f); // 벽타기 중지
	else if (OwnerCreature->GetCharacterMovement()->IsFalling() == false) // 지면에 있으면
		OwnerCreature->Crouch(); // 앉기 시작
}

void UAdvanceMovementComponent::UnCrouch()
{
	if (bIsSliding) // 슬라이딩 중이면
		SlideEnd(true); // 슬라이딩 중지
	else if (OwnerCreature->GetCharacterMovement()->IsFalling() == false) // 지면에 있으면
		OwnerCreature->UnCrouch(); // 일어나기
}

void UAdvanceMovementComponent::WallRunJump()
{
	if (IsWallRunning()) // 벽타기 중이면
	{
		WallRunEnd(0.35); // 벽타기 중지
		
		// 벽타기 점프 방향 구하기
		FVector LaunchVector;
		LaunchVector.X = WallRunNormal.X * WallRunJumpOffForce;
		LaunchVector.Y = WallRunNormal.Y * WallRunJumpOffForce;
		LaunchVector.Z = WallRunJumpHeight;
		OwnerCreature->LaunchCharacter(LaunchVector, false, true);
		OwnerCreature->OnJumped();

		Server_WallRunJumpSuccess(OwnerCreature->GetCharacterMovement()->PendingLaunchVelocity);
	}
	else
	{
		KR_LOG(Warning, TEXT("IsWallRunning is false"))
	}
}

void UAdvanceMovementComponent::DoubleJump()
{
	bCanDoubleJump = false;

	// 더블점프 힘 계산
	FVector LaunchPower(0, 0, DobuleJumpPower.Z);

	UCharacterMovementComponent* MovementComp = OwnerCreature->GetCharacterMovement();

	FVector Velocity = MovementComp->Velocity;

	FVector Right = OwnerCreature->GetControlRotation().Vector().GetSafeNormal2D().RotateAngleAxis(-90.0f, FVector::UpVector);
	float RightSpeed = -FVector::DotProduct(Velocity, Right) / Right.Size2D();

	FVector Forward = OwnerCreature->GetControlRotation().Vector().GetSafeNormal2D();
	float ForwardSpeed = FVector::DotProduct(Velocity, Forward) / Forward.Size2D();

	float ForwardLaunchPower;
	float RightLaunchPower;

	if (OwnerCreature->GetCurrentInputForward() > 0.f && ForwardSpeed > DobuleJumpPower.X * OwnerCreature->GetCurrentInputForward())
		ForwardLaunchPower = ForwardSpeed;
	else if (OwnerCreature->GetCurrentInputForward() < 0.f && -ForwardSpeed > DobuleJumpPower.X * -OwnerCreature->GetCurrentInputForward())
		ForwardLaunchPower = ForwardSpeed;
	else
		ForwardLaunchPower = DobuleJumpPower.X * OwnerCreature->GetCurrentInputForward();

	if (OwnerCreature->GetCurrentInputRight() > 0.f && RightSpeed > DobuleJumpPower.Y * OwnerCreature->GetCurrentInputRight())
		RightLaunchPower = RightSpeed;
	else if (OwnerCreature->GetCurrentInputRight() < 0.f && -RightSpeed > DobuleJumpPower.Y * -OwnerCreature->GetCurrentInputRight())
		RightLaunchPower = RightSpeed;
	else
		RightLaunchPower = DobuleJumpPower.Y * OwnerCreature->GetCurrentInputRight();

	bool bOverideXY = false;
	if (OwnerCreature->GetCurrentInputForward() != 0.f)
	{
		FRotator Rotation = OwnerCreature->GetController()->GetControlRotation();
		FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		LaunchPower += FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) * ForwardLaunchPower;
		bOverideXY = true;
	}
	if (OwnerCreature->GetCurrentInputRight() != 0.f)
	{
		FRotator Rotation = OwnerCreature->GetController()->GetControlRotation();
		FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		LaunchPower += FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) * RightLaunchPower;
		bOverideXY = true;
	}

	OwnerCreature->LaunchCharacter(LaunchPower, bOverideXY, true);
	OwnerCreature->OnJumped();

	Server_DoubleJump(OwnerCreature->GetCharacterMovement()->PendingLaunchVelocity);

	KR_LOG(Log, TEXT("Dobule Jump Power : %f %f"), ForwardLaunchPower, RightLaunchPower);
}

bool UAdvanceMovementComponent::WallRunUpdate()
{
	if(CurWallRunState != EWallRunState::WALLRUN_VERTICAL) // 세로 벽타기 중이면
		CurWallRunVerticalSpeed = FMath::FInterpTo(CurWallRunVerticalSpeed, MaxWallRunVerticalSpeed, GetWorld()->GetDeltaSeconds(), 5.f); // 세로 벽타기 속도 줄이기

	if (OwnerCreature->GetbJumpButtonPress() || IsWallRunning()) // 점프 버튼을 누르고있고 벽타기 중이면
	{
		bool WallRunVerticalSuccess = false;
		bool WallRunHorizonSuccess = false;

		if (!bWallRunVerticalSupressed) // 세로 벽타기가 가능한지
		{
			WallRunVerticalSuccess = WallRunVerticalUpdate(); // 세로 벽타기 시작
			if (WallRunVerticalSuccess) // 벽타기 성공했을때
			{
				Server_WallRunVerticalSuccess();
				Server_WallRunSuccess(OwnerCreature->GetActorLocation(), OwnerCreature->GetActorRotation(), OwnerCreature->GetCharacterMovement()->Velocity, OwnerCreature->GetCharacterMovement()->PendingLaunchVelocity);
				return true;
			}
		}

		if (!bWallRunHorizonSupressed) // 가로 벽타기가 가능한지
		{
			WallRunHorizonSuccess = WallRunHorizonUpdate(); // 가로 벽타기 시작
			if (WallRunHorizonSuccess) // 벽타기 성공했을때
			{
				if (bWallRunGravity) // 벽타기 중 개별적인 중력을 적용할지
					OwnerCreature->GetCharacterMovement()->GravityScale = WallRunTargetGravity; // 중력 적용

				// 벽타기 중 아래로 내려가지 않도록 속도 조정
				if (OwnerCreature->GetVelocity().Z < 0.f) 
				{
					OwnerCreature->GetCharacterMovement()->GravityScale = 0.f;
					OwnerCreature->GetCharacterMovement()->PendingLaunchVelocity.Z = 0.f;
				}

				Server_WallRunHorizonSuccess();
				Server_WallRunSuccess(OwnerCreature->GetActorLocation(), OwnerCreature->GetActorRotation(), OwnerCreature->GetCharacterMovement()->Velocity, OwnerCreature->GetCharacterMovement()->PendingLaunchVelocity);
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
	else if (CurWallRunState != EWallRunState::WALLRUN_VERTICAL)
	{
		// 왼쪽 벽타기 중이였지만 왼쪽 벽타기를 실패한경우
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
		if (OwnerCreature->GetMovementComponent()->IsFalling() && CalculateValidWallVector(Result.Normal))
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

	FVector Velocity = MovementComp->Velocity;

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
	OwnerCreature->GetCharacterMovement()->GroundFriction = SlideGroundFriction;
	OwnerCreature->GetCharacterMovement()->BrakingDecelerationWalking = SlideBrakingDecelerationWalking;
	OwnerCreature->GetCharacterMovement()->Velocity.X = SlidePower.X;
	OwnerCreature->GetCharacterMovement()->Velocity.Y = SlidePower.Y;
	OwnerCreature->Crouch(true);

	Server_SlideSuccess(OwnerCreature->GetCharacterMovement()->Velocity);

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
	OwnerCreature->GetCharacterMovement()->GroundFriction = DefaultGroundFriction;
	OwnerCreature->GetCharacterMovement()->BrakingDecelerationWalking = DefaultBrakingDecelerationWalking;
	
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
		if (OwnerCreature->GetMovementComponent()->IsFalling() && CalculateValidWallVector(Result.Normal))
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

	bCanDoubleJump = true;
	CurWallRunState = State;
	Server_WallRunStart(State);
}

void UAdvanceMovementComponent::WallRunEnd(float ResetTime)
{
	if (IsWallRunning() == false)
		return;

	CurWallRunState = EWallRunState::NONE;
	OwnerCreature->GetCharacterMovement()->GravityScale = DefaultGravity;
	SuppressWallRunHorizion(ResetTime);
	SuppressWallRunVertical(ResetTime);
	Server_WallRunEnd();
}

void UAdvanceMovementComponent::WallRunHorizonEnd(float ResetTime)
{
	if (IsWallRunning() == false)
		return;

	CurWallRunState = EWallRunState::NONE;
	OwnerCreature->GetCharacterMovement()->GravityScale = DefaultGravity;
	SuppressWallRunHorizion(ResetTime);
	Server_WallRunEnd();
}

void UAdvanceMovementComponent::WallRunVerticalEnd(float ResetTime)
{
	if (IsWallRunning() == false)
		return;

	CurWallRunState = EWallRunState::NONE;
	OwnerCreature->GetCharacterMovement()->GravityScale = DefaultGravity;
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

	if (MovementState != EMovementState::SPRINT)
		return false;

	if (OwnerCreature->CalculateForwardSpeed() < MinSlideRequireSpeed)
		return false;

	return true;
}

void UAdvanceMovementComponent::SlideUpdate()
{
	if (!bIsSliding)
		return;

	float Speed = OwnerCreature->GetVelocity().Size();
	if (Speed < 20.f) 
	{
		SlideEnd(false);
		return;
	}

	FVector Slope = OwnerCreature->CaclulateCurrentFllorSlopeVector();
	FVector SlopePhysics = Slope * SlideSlopeSpeed;
	SlopePhysics.Z = 0.f;
	OwnerCreature->GetCharacterMovement()->Velocity += SlopePhysics;

	Server_SlideUpdate(OwnerCreature->GetActorLocation(), OwnerCreature->GetCharacterMovement()->Velocity);
}

void UAdvanceMovementComponent::Server_WallRunJumpSuccess_Implementation(FVector PendingLaunchVelocity)
{
	OwnerCreature->GetCharacterMovement()->PendingLaunchVelocity = PendingLaunchVelocity;

	Multicast_WallRunJumpSuccess(PendingLaunchVelocity);
}

void UAdvanceMovementComponent::Multicast_WallRunJumpSuccess_Implementation(FVector PendingLaunchVelocity)
{
	UCreatureAnimInstance* CreatureAnim = Cast<UCreatureAnimInstance>(OwnerCreature->GetMesh()->GetAnimInstance());
	OwnerCreature->GetMesh()->GetAnimInstance()->Montage_Play(CreatureAnim->GetJumpMontage());
}

void UAdvanceMovementComponent::Server_WallRunEnd_Implementation()
{
	CurWallRunState = EWallRunState::NONE;
	OwnerCreature->GetCharacterMovement()->GravityScale = DefaultGravity;
}

void UAdvanceMovementComponent::Server_DoubleJump_Implementation(FVector PendingLaunchVelocity)
{
	OwnerCreature->GetCharacterMovement()->PendingLaunchVelocity = PendingLaunchVelocity;
	Multicast_DoubleJump(PendingLaunchVelocity);
}

void UAdvanceMovementComponent::Multicast_DoubleJump_Implementation(FVector PendingLaunchVelocity)
{
	UCreatureAnimInstance* CreatureAnim = Cast<UCreatureAnimInstance>(OwnerCreature->GetMesh()->GetAnimInstance());
	OwnerCreature->GetMesh()->GetAnimInstance()->Montage_Play(CreatureAnim->GetJumpMontage());
}

void UAdvanceMovementComponent::Server_WallRunHorizonSuccess_Implementation()
{
	if (bWallRunGravity)
		OwnerCreature->GetCharacterMovement()->GravityScale = WallRunTargetGravity;
	if (OwnerCreature->GetVelocity().Z < 0.f)
	{
		OwnerCreature->GetCharacterMovement()->GravityScale = 0.f;
		OwnerCreature->GetCharacterMovement()->PendingLaunchVelocity.Z = 0.f;
	}
}

void UAdvanceMovementComponent::Server_WallRunVerticalSuccess_Implementation()
{
	OwnerCreature->GetCharacterMovement()->GravityScale = 0.f;
}

void UAdvanceMovementComponent::Server_WallRunSuccess_Implementation(FVector Location, FRotator Rotation, FVector Velocity, FVector PendingLaunchVelocity)
{
	OwnerCreature->SetActorLocation(Location);
	OwnerCreature->SetActorRotation(Rotation);
	OwnerCreature->GetCharacterMovement()->Velocity = Velocity;
	OwnerCreature->GetCharacterMovement()->PendingLaunchVelocity = PendingLaunchVelocity;
}

void UAdvanceMovementComponent::Server_SlideSuccess_Implementation(FVector Velocity)
{
	bIsSliding = true;

	OwnerCreature->GetCharacterMovement()->GroundFriction = SlideGroundFriction;
	OwnerCreature->GetCharacterMovement()->BrakingDecelerationWalking = SlideBrakingDecelerationWalking;
	OwnerCreature->GetCharacterMovement()->Velocity = Velocity;
}

void UAdvanceMovementComponent::Server_SlideEnd_Implementation()
{
	bIsSliding = false;

	OwnerCreature->GetCharacterMovement()->GroundFriction = DefaultGroundFriction;
	OwnerCreature->GetCharacterMovement()->BrakingDecelerationWalking = DefaultBrakingDecelerationWalking;
}

void UAdvanceMovementComponent::Server_SlideUpdate_Implementation(FVector Location, FVector Velocity)
{
	OwnerCreature->SetActorLocation(Location);
	OwnerCreature->GetCharacterMovement()->Velocity = Velocity;
}

bool UAdvanceMovementComponent::IsFalling()
{
	if (CurWallRunState != EWallRunState::NONE)
		return false;

	return Super::IsFalling();
}
