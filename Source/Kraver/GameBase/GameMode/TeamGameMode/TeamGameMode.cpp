// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamGameMode.h"
#include TeamGameState_h

ATeamGameMode::ATeamGameMode()
{
	GameStateClass = ATeamGameState::StaticClass();
}

void ATeamGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	TeamGameState = Cast<ATeamGameState>(GameState);
}

void ATeamGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	DivideTeam(NewPlayer);
}
