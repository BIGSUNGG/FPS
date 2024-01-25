// Fill out your copyright notice in the Description page of Project Settings.


#include "CreatureMovementComponent.h"
#include Creature_h

// Sets default values for this component's properties
UCreatureMovementComponent::UCreatureMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UCreatureMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UCreatureMovementComponent, WalkState, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UCreatureMovementComponent, bIsJumping, COND_SkipOwner);
}

// Called when the game starts
void UCreatureMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OwnerCreature = Cast<ACreature>(GetOwner());
	if (OwnerCreature == nullptr)
	{
		KR_LOG(Error, TEXT("Owner is not ACreature class"));
		return;
	}

	AirControl = 0.25f;
	MaxWalkSpeed = GetWalkSpeed();
	MaxWalkSpeedCrouched = GetCrouchWalkSpeed();
	NavAgentProps.bCanCrouch = true;
	bCanWalkOffLedgesWhenCrouching = true;
	SetWalkableFloorAngle(50.f);

	DefaultGravity = GravityScale;
	DefaultGroundFriction = GroundFriction;
	DefaultBrakingDecelerationWalking = BrakingDecelerationWalking;
}

// Called every frame
void UCreatureMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCreatureMovementComponent::Landed(const FHitResult& Hit)
{
	bIsJumping = false;
}

void UCreatureMovementComponent::MoveForward(float NewAxisValue)
{
	if(!OwnerCreature)
		return;

	NewAxisValue *= InputForwardRatio;
	if (!OwnerCreature->GetMovementComponent()->IsFalling())
	{
		SetMovementState(EMovementState::WALK);

		if (NewAxisValue == 0 || OwnerCreature->Controller == nullptr)
			return;

		if (OwnerCreature->GetbRunButtonPress() && OwnerCreature->CanRun())
		{
			if (NewAxisValue >= 0.5f)
				SetMovementState(EMovementState::SPRINT);
			else
				SetMovementState(EMovementState::RUN);
		}
	}

	FRotator Rotation = OwnerCreature->GetController()->GetControlRotation();
	FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	OwnerCreature->AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), NewAxisValue);
}

void UCreatureMovementComponent::MoveRight(float NewAxisValue)
{
	if (!OwnerCreature)
		return;

	NewAxisValue *= InputRightRatio;
	if (NewAxisValue == 0 || OwnerCreature->Controller == nullptr)
		return;

	FRotator Rotation = OwnerCreature->GetController()->GetControlRotation();
	FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	OwnerCreature->AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), NewAxisValue);
}

void UCreatureMovementComponent::JumpStart()
{
	if (OwnerCreature->GetMovementComponent()->IsFalling())
		return;

	bIsJumping = true;
	OwnerCreature->Jump();
}

void UCreatureMovementComponent::JumpEnd()
{
	OwnerCreature->StopJumping();
}

void UCreatureMovementComponent::CrouchStart()
{
	OwnerCreature->Crouch();
}

void UCreatureMovementComponent::CrouchEnd()
{
	OwnerCreature->UnCrouch();
}

void UCreatureMovementComponent::SetMovementState(EMovementState value)
{
	WalkState = value;
	switch (value)
	{
	case EMovementState::WALK:
		OwnerCreature->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		OwnerCreature->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchWalkSpeed;
		break;
	case EMovementState::RUN:
		OwnerCreature->GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		OwnerCreature->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchRunSpeed;
		break;
	case EMovementState::SPRINT:
		OwnerCreature->GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		OwnerCreature->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSprintSpeed;
		break;
	default:
		break;
	}
	Server_SetMovementState(value);
}

void UCreatureMovementComponent::Server_SetMovementState_Implementation(EMovementState value)
{
	WalkState = value;
	switch (value)
	{
	case EMovementState::WALK:
		OwnerCreature->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		OwnerCreature->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchWalkSpeed;
		break;
	case EMovementState::RUN:
		OwnerCreature->GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		OwnerCreature->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchRunSpeed;
		break;
	case EMovementState::SPRINT:
		OwnerCreature->GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		OwnerCreature->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSprintSpeed;
		break;
	default:
		break;
	}
}