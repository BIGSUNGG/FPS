// Fill out your copyright notice in the Description page of Project Settings.


#include "CreatureAnimInstance.h"
#include Creature_h
#include CreatureMovementComponent_h

UCreatureAnimInstance::UCreatureAnimInstance()
{
}

void UCreatureAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	auto Pawn = TryGetPawnOwner();
	Creature = Cast<ACreature>(Pawn);
}

void UCreatureAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(!Creature)
		return;

	UCharacterMovementComponent* MovementComp = Creature->GetCharacterMovement();

	Velocity = MovementComp->Velocity;
	UpDownSpeed = Velocity.Z;

	FVector Right = Creature->GetBaseAimRotation().Vector().GetSafeNormal2D().RotateAngleAxis(-90.0f, FVector::UpVector);
	RightSpeed = -FVector::DotProduct(Velocity, Right) / Right.Size2D();

	FVector Forward = Creature->GetBaseAimRotation().Vector().GetSafeNormal2D();
	ForwardSpeed = FVector::DotProduct(Velocity, Forward) / Forward.Size2D();

	FVector TempVelocity = Velocity;
	TempVelocity.Z = 0.f;

	Speed = TempVelocity.Size();
	SpeedRatio = Speed / Creature->GetMovementComponent()->GetMaxSpeed();

	MovementState = Creature->CreatureMovementComponent->GetMovementState();
	IsCrouching = Creature->GetMovementComponent()->IsCrouching();
	IsFalling = Creature->GetMovementComponent()->IsFalling();
	IsRunning = Creature->GetbRunButtonPress();
	IsJumping = Creature->CreatureMovementComponent->IsJumping();

	FRotator CreatureRotation = Creature->GetCreatureAngle();

	TurningInPlace = Creature->GetTurningInPlace();
	StartingAimRotation = Creature->GetStartingAimRotation();

	AO_Yaw = Creature->GetAO_Yaw();
	AO_Pitch = Creature->GetAO_Pitch();
}

void UCreatureAnimInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UCreatureAnimInstance::PlayLandedMontage()
{
	Montage_Play(LandedMontage);
}
