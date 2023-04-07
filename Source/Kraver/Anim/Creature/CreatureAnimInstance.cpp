// Fill out your copyright notice in the Description page of Project Settings.


#include "CreatureAnimInstance.h"
#include "Kraver/Creature/Creature.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"

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

	FVector Velocity = MovementComp->Velocity;
	UpDownSpeed = Velocity.Z;

	FVector Right = Creature->GetControlRotation().Vector().GetSafeNormal2D().RotateAngleAxis(-90.0f, FVector::UpVector);
	RightSpeed = -FVector::DotProduct(Velocity, Right) / Right.Size2D();

	FVector Forward = Creature->GetControlRotation().Vector().GetSafeNormal2D();
	ForwardSpeed = FVector::DotProduct(Velocity, Forward) / Forward.Size2D();

	Velocity.Z = 0;
	Speed = Velocity.Size();
	SpeedRatio = Speed / Creature->GetMovementComponent()->GetMaxSpeed();

	MovementState = Creature->GetMovementState();
	IsCrouching = Creature->GetMovementComponent()->IsCrouching();
	IsFalling = Creature->GetMovementComponent()->IsFalling();
	IsRunning = Creature->GetIsRunning();
	IsJumping = Creature->GetIsJumping();

	FRotator CreatureRotation = Creature->GetCreatureAngle();
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
