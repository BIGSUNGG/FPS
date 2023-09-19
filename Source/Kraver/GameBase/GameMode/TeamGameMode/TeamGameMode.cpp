// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamGameMode.h"
#include KraverPlayer_h
#include TeamGameState_h
#include TeamPlayerStart_h

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

AActor* ATeamGameMode::FindRespawnPoint(AKraverPlayer* RespawnPlayer)
{
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, ATeamPlayerStart::StaticClass(), PlayerStarts);

	TArray<AActor*> CanSpawnStarts;
	for (auto& Start : PlayerStarts)
	{
		ATeamPlayerStart* TeamStart = Cast<ATeamPlayerStart>(Start);
		if (!TeamStart)
			continue;;

		if (FTeamInfo::CheckIsTeam(TeamStart->GetCanSpawnTeam(), RespawnPlayer->CombatComponent->GetCurTeamInfo().CurTeam))
			CanSpawnStarts.Add(TeamStart);
	}

	if (CanSpawnStarts.Num() > 0)
	{
		int32 Selection = FMath::RandRange(0, CanSpawnStarts.Num() - 1);
		return CanSpawnStarts[Selection];
	}

	KR_LOG(Error, TEXT("ATeamPlayerStart is not exist"));
	return Super::FindRespawnPoint(RespawnPlayer);
}