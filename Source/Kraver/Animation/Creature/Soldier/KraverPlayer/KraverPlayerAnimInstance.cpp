// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverPlayerAnimInstance.h"
#include KraverPlayer_h
#include AdvanceMovementComponent_h

void UKraverPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	KraverPlayer = Cast<AKraverPlayer>(Soldier);
}

void UKraverPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (KraverPlayer == nullptr)
		return;

	TurningInPlace = KraverPlayer->GetTurningInPlace();

	UAdvanceMovementComponent* AdvanceMovementComp = Cast<UAdvanceMovementComponent>(KraverPlayer->CreatureMovementComponent);
	if(AdvanceMovementComp)
	{
		CurWallRunState = AdvanceMovementComp->GetCurWallRunState();
		IsWallRunning = (CurWallRunState != EWallRunState::NONE);
		IsSliding = AdvanceMovementComp->IsSliding();
	}
}
