// Fill out your copyright notice in the Description page of Project Settings.


#include "CreatureAnimInstance.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"

UCreatureAnimInstance::UCreatureAnimInstance()
{
	
}

void UCreatureAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if(!::IsValid(Pawn))
		return;

	Creature = Cast<ACreature>(Pawn);
	if(!Creature)
		return;

	FVector Velocity = Creature->GetVelocity();
	UpDownSpeed = Velocity.Z;
	Velocity.Z = 0;
	Speed = Velocity.Size();
	SpeedRatio = Speed / Creature->GetMovementComponent()->GetMaxSpeed();
	
	if (Creature->GetVelocity().Length() <= 0)
		MovementDirection = 0.f;
	else
	{
		FRotator RotFromX = UKismetMathLibrary::MakeRotFromX(Creature->GetVelocity());
		FRotator TempRotation = UKismetMathLibrary::NormalizedDeltaRotator(RotFromX, Creature->GetActorRotation());
		if(TempRotation.Yaw >= 180)
			MovementDirection = TempRotation.Yaw - 360.f;
		else
			MovementDirection = TempRotation.Yaw;
	}


	IsCrouching = Creature->GetMovementComponent()->IsCrouching();
	IsFalling = Creature->GetMovementComponent()->IsFalling();
	IsRunning = Creature->GetIsRunning();
	IsSprint = Creature->GetIsSprint();
	IsJumping = Creature->GetIsJumping();

	FRotator CreatureRotation = Creature->GetCreatureAngle();
	AO_Yaw = Creature->GetAO_Yaw();
	AO_Pitch = Creature->GetAO_Pitch();
}

void UCreatureAnimInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

