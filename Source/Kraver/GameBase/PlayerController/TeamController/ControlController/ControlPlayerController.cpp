// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlPlayerController.h"
#include ControlGameState_h

void AControlPlayerController::BeginPlay()
{
	Super::BeginPlay();


}

void AControlPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	KR_LOG(Log, TEXT("Hd"));

	if (ControlGameState)
		ControlGameState->InterpTick(DeltaSeconds);
	else if (GetWorld())
		ControlGameState = GetWorld()->GetGameState<AControlGameState>();
}
