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
	DOREPLIFETIME_CONDITION(UAdvanceMovementComponent, IsSliding, COND_SkipOwner);
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
	if (OwnerCreature->GetMovementComponent()->IsFalling() == false)
	{
		if (IsSliding)
		{
			SlideEnd(true);
			bWantToJump = true;
		}
		else if (OwnerCreature->GetCharacterMovement()->IsCrouching())
		{
			UnCrouch();
			bWantToJump = true;
		}
		Super::JumpStart();
	}
	else if (GetIsWallRunning())
		WallRunJump();
	else if (bCanDoubleJump)
		DoubleJump();
}

void UAdvanceMovementComponent::JumpEnd()
{

}

void UAdvanceMovementComponent::Crouch()
{
	if (OwnerCreature->GetMovementComponent()->IsFalling() == false && CanSlide())
		SlideStart();
	else if (GetIsWallRunning())
		WallRunEnd(0.3f);
	else if (OwnerCreature->GetCharacterMovement()->IsFalling() == false)
		OwnerCreature->Crouch();
}

void UAdvanceMovementComponent::UnCrouch()
{
	if (IsSliding)
		SlideEnd(true);
	else if (OwnerCreature->GetCharacterMovement()->IsFalling() == false)
		OwnerCreature->UnCrouch();
}

void UAdvanceMovementComponent::WallRunJump()
{
	if (GetIsWallRunning())
	{
		WallRunEnd(0.35);
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
	if(CurWallRunState != EWallRunState::WALLRUN_VERTICAL)
		CurWallRunVerticalSpeed = FMath::FInterpTo(CurWallRunVerticalSpeed, MaxWallRunVerticalSpeed, GetWorld()->GetDeltaSeconds(), 5.f);

	if (OwnerCreature->GetbJumpButtonPress() || GetIsWallRunning())
	{
		bool WallRunVerticalSuccess = false;
		bool WallRunHorizonSuccess = false;

		if (!bWallRunVerticalSupressed)
		{
			WallRunVerticalSuccess = WallRunVerticalUpdate();
			if (WallRunVerticalSuccess)
			{
				GEngine->AddOnScreenDebugMessage(
					0,
					0.f,
					FColor::White,
					TEXT("WallRunVertical")
				);
				Server_WallRunVerticalSuccess();
				Server_WallRunSuccess(OwnerCreature->GetActorLocation(), OwnerCreature->GetActorRotation(), OwnerCreature->GetCharacterMovement()->Velocity, OwnerCreature->GetCharacterMovement()->PendingLaunchVelocity);
				return true;
			}
		}

		if (!bWallRunHorizonSupressed)
		{
			WallRunHorizonSuccess = WallRunHorizonUpdate();
			if (WallRunHorizonSuccess)
			{
				if (bWallRunGravity)
					OwnerCreature->GetCharacterMovement()->GravityScale = WallRunTargetGravity;
				if (OwnerCreature->GetVelocity().Z < 0.f)
				{
					OwnerCreature->GetCharacterMovement()->GravityScale = 0.f;
					OwnerCreature->GetCharacterMovement()->PendingLaunchVelocity.Z = 0.f;
				}

				GEngine->AddOnScreenDebugMessage(
					0,
					0.f,
					FColor::White,
					TEXT("WallRunHorizon")
				);

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
	if (WallRunHorizonMovement(OwnerCreature->GetActorLocation(), CalculateRightWallRunEndVector(), -1.f))
	{
		if (!GetIsWallRunning())
			WallRunStart(EWallRunState::WALLRUN_RIGHT);

		return true;
	}
	else if (CurWallRunState == EWallRunState::WALLRUN_RIGHT)
	{
		WallRunHorizonEnd(1.f);
		return false;
	}
	else if (WallRunHorizonMovement(OwnerCreature->GetActorLocation(), CalculateLeftWallRunEndVector(), 1.f))
	{
		if (!GetIsWallRunning())
			WallRunStart(EWallRunState::WALLRUN_LEFT);

		return true;
	}
	else if (CurWallRunState != EWallRunState::WALLRUN_VERTICAL)
	{
		WallRunHorizonEnd(1.f);
		return false;

	}

	return false;
}

bool UAdvanceMovementComponent::WallRunVerticalUpdate()
{
	if (bWallRunVerticalSupressed)
		return false;

	if (CurWallRunVerticalSpeed <= 100.f)
	{
		WallRunVerticalEnd(2.f);
		return false;
	}

	if (WallRunVerticalMovement(OwnerCreature->GetActorLocation(), CalculateVerticaltWallRunEndVector()))
	{
		if (!GetIsWallRunning())
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

	IsSliding = true;

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
	if (!IsSliding || bSlideSupressed)
		return;

	IsSliding = false;

	InputForwardRatio = 1.f;
	InputRightRatio = 1.f;
	OwnerCreature->GetCharacterMovement()->GroundFriction = DefaultGroundFriction;
	OwnerCreature->GetCharacterMovement()->BrakingDecelerationWalking = DefaultBrakingDecelerationWalking;

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
	if (GetIsWallRunning())
		return;

	bCanDoubleJump = true;
	CurWallRunState = State;
	Server_WallRunStart(State);
}

void UAdvanceMovementComponent::WallRunEnd(float ResetTime)
{
	if (GetIsWallRunning() == false)
		return;

	CurWallRunState = EWallRunState::NONE;
	OwnerCreature->GetCharacterMovement()->GravityScale = DefaultGravity;
	SuppressWallRunHorizion(ResetTime);
	SuppressWallRunVertical(ResetTime);
	Server_WallRunEnd();
}

void UAdvanceMovementComponent::WallRunHorizonEnd(float ResetTime)
{
	if (GetIsWallRunning() == false)
		return;

	CurWallRunState = EWallRunState::NONE;
	OwnerCreature->GetCharacterMovement()->GravityScale = DefaultGravity;
	SuppressWallRunHorizion(ResetTime);
	Server_WallRunEnd();
}

void UAdvanceMovementComponent::WallRunVerticalEnd(float ResetTime)
{
	if (GetIsWallRunning() == false)
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
		OwnerCreature->GetActorRightVector() * 100.f +
		OwnerCreature->GetActorForwardVector() * -50.f
		);
}

FVector UAdvanceMovementComponent::CalculateLeftWallRunEndVector()
{
	return (
		OwnerCreature->GetActorLocation() +
		OwnerCreature->GetActorRightVector() * -100.f +
		OwnerCreature->GetActorForwardVector() * -50.f
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

FVector UAdvanceMovementComponent::CalculatePlayerToWallVector()
{
	return -WallRunNormal;
}

bool UAdvanceMovementComponent::CanSlide()
{
	if (bSlideSupressed)
		return false;

	if (IsSliding)
		return false;

	if (MovementState != EMovementState::SPRINT)
		return false;

	if (OwnerCreature->CalculateForwardSpeed() < MinSlideRequireSpeed)
		return false;

	return true;
}

void UAdvanceMovementComponent::SlideUpdate()
{
	if (!IsSliding)
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

	Server_SlideUpdate(OwnerCreature->GetCharacterMovement()->Velocity);
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
	IsSliding = true;

	OwnerCreature->GetCharacterMovement()->GroundFriction = SlideGroundFriction;
	OwnerCreature->GetCharacterMovement()->BrakingDecelerationWalking = SlideBrakingDecelerationWalking;
	OwnerCreature->GetCharacterMovement()->Velocity = Velocity;
}

void UAdvanceMovementComponent::Server_SlideEnd_Implementation()
{
	IsSliding = false;

	OwnerCreature->GetCharacterMovement()->GroundFriction = DefaultGroundFriction;
	OwnerCreature->GetCharacterMovement()->BrakingDecelerationWalking = DefaultBrakingDecelerationWalking;
}

void UAdvanceMovementComponent::Server_SlideUpdate_Implementation(FVector Velocity)
{
	OwnerCreature->GetCharacterMovement()->Velocity = Velocity;
}
