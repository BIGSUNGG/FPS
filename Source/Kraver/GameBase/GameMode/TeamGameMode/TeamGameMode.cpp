// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamGameMode.h"
#include KraverPlayer_h
#include TeamGameState_h
#include TeamPlayerState_h
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

void ATeamGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	TeamGameState = TeamGameState ? TeamGameState : GetGameState<ATeamGameState>();
	TeamGameState->RemoveTeamPlayer(Exiting->GetPlayerState<ATeamPlayerState>());
}

FString ATeamGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal /*= TEXT("")*/)
{
	// 플레이어 팀 나누기
	DivideTeam(NewPlayerController);

	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

AActor* ATeamGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	ATeamPlayerState* PlayerState = Player->GetPlayerState<ATeamPlayerState>();
	if (!PlayerState)
	{
		KR_LOG(Error, TEXT("PlayerState is nullptr"));
		return Super::FindPlayerStart_Implementation(Player, IncomingName);
	}

	// 플레이어 스폰 위치 구하기
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, ATeamPlayerStart::StaticClass(), PlayerStarts);

	// 스폰될 수 있는 스폰 위치 구하기
	TArray<AActor*> CanSpawnStarts;
	for (auto& Start : PlayerStarts)
	{
		ATeamPlayerStart* TeamStart = Cast<ATeamPlayerStart>(Start);
		if (!TeamStart)
			continue;;

		if (FTeamInfo::CheckIsTeam(TeamStart->GetCanSpawnTeam(), PlayerState->GetPlayerTeam()))
			CanSpawnStarts.Add(TeamStart);
	}

	// 스폰될 수 있는 스폰 위치에서 랜덤으로 스폰 위치 구하기
	if (CanSpawnStarts.Num() > 0)
	{
		int32 Selection = FMath::RandRange(0, CanSpawnStarts.Num() - 1);
		return CanSpawnStarts[Selection];
	}

	KR_LOG(Error, TEXT("ATeamPlayerStart is not exist"));
	return Super::FindPlayerStart_Implementation(Player, IncomingName);
}
