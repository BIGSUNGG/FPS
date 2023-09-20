// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverGameMode.h"
#include KraverPlayer_h
#include KraverSpectator_h
#include KraverGameState_h
#include KraverPlayerState_h
#include KraverPlayerController_h

AKraverGameMode::AKraverGameMode()
{
	GameStateClass = AKraverGameState::StaticClass();
	PlayerStateClass = AKraverPlayerState::StaticClass();
}

void AKraverGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	KraverGameState = Cast<AKraverGameState>(GameState);
}

void AKraverGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
}

AActor* AKraverGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	KR_LOG(Log, TEXT("FindPlayerStart_Implementation"));
	return FindSpawnPoint(Player);
}

void AKraverGameMode::CreatureDeath(class ACreature* DeadCreature, class AController* VictimController, AActor* AttackerActor, AController* AttackerController, FKraverDamageResult const& DamageResult)
{
	AKraverGameState* KraverPlayerState = Cast<AKraverGameState>(GameState);
	if (KraverPlayerState)
	{
		KraverPlayerState->CreatureDeath(DeadCreature, VictimController, AttackerActor, AttackerController, DamageResult);
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("GameState Class is not KraerGameState Class"));
		return;
	}

	if (DeadCreature->IsPlayerControlled() && bRespawn)
	{
		AKraverPlayer* PlayerCharacter = Cast<AKraverPlayer>(DeadCreature);
		if(PlayerCharacter)
		{
			GetWorldTimerManager().SetTimer(
				RespawnTimer,
				[=]() { RequestRespawn(PlayerCharacter, VictimController); },
				RespawnTime,
				false,
				RespawnTime
			);
		}
	}

	if (bSpectate)
	{
		GetWorldTimerManager().SetTimer(
			SpectateTimer,
			[=]() { RequsetSpectate(VictimController); },
			SpectateStartTime,
			false,
			SpectateStartTime
		);
	}
}

void AKraverGameMode::RequestRespawn(AKraverPlayer* RespawnPlayer, AController* PlayerController)
{
	if (!RespawnPlayer || !PlayerController)
		return;

	AKraverPlayerController* KraverController = Cast<AKraverPlayerController>(PlayerController);
	if (!KraverController)
		return;

	KR_LOG(Log, TEXT("Player Respawn"));

	RespawnPlayer->Reset();
	RespawnPlayer->Destroy();

	RestartPlayerAtPlayerStart(KraverController, FindSpawnPoint(PlayerController));
	
}

void AKraverGameMode::RequsetSpectate(AController* PlayerController)
{
	KR_LOG(Log, TEXT("Spectate Start"));

	AKraverPlayerController* KraverController = Cast<AKraverPlayerController>(PlayerController);
	if (!KraverController)
		return;

	//AKraverSpectator* Spectator = Cast<AKraverSpectator>(GetWorld()->SpawnActor(SpectatorClass));

}

void AKraverGameMode::GameFinishEvent(ETeam WinTeam)
{
	if (IsGameFinish)
		return;

	IsGameFinish = true;
	KR_LOG(Log, TEXT("Game finish"));
}

AActor* AKraverGameMode::FindSpawnPoint(AController* PlayerController)
{
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
	int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
	return PlayerStarts[Selection];
}