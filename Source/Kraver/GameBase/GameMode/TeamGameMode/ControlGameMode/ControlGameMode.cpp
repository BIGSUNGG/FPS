// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlGameMode.h"
#include ControlGameState_h
#include KraverPlayer_h

void AControlGameMode::DivideTeam(APlayerController* InPlayer)
{
	Super::DivideTeam(InPlayer);

	ControlGameState = ControlGameState ? ControlGameState : GetGameState<AControlGameState>();

	if (!ControlGameState)
	{
		GetWorldTimerManager().SetTimerForNextTick([=]() { DivideTeam(InPlayer); });
		return;
	}

\	const TArray<APlayerState*>& BlueTeamArr = ControlGameState->GetTeamArray(ETeam::BLUE);
	const TArray<APlayerState*>& RedTeamArr = ControlGameState->GetTeamArray(ETeam::RED);

	ETeam NewTeam;
	if (BlueTeamArr.Num() > RedTeamArr.Num())
		NewTeam = ETeam::RED;
	else
		NewTeam = ETeam::RED;

	if (AKraverPlayer* KraverPlayer = Cast<AKraverPlayer>(InPlayer->GetPawn()))
	{
		KraverPlayer->CombatComponent->SetTeam(NewTeam);
	}
}
