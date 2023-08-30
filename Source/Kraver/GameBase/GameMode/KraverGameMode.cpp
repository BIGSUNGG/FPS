// Fill out your copyright notice in the Description page of Project Settings.


#include "KraverGameMode.h"
#include "Kraver/Character/Creature/Creature.h"
#include "Kraver/GameBase/GameState/KraverGameState.h"
#include "Kraver/GameBase/PlayerState/KraverPlayerState.h"

AKraverGameMode::AKraverGameMode()
{
	GameStateClass = AKraverGameState::StaticClass();
	PlayerStateClass = AKraverPlayerState::StaticClass();
}

void AKraverGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

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
}

void AKraverGameMode::RequestRespawn(ACreature* ElimmedCharacter, AController* ElimmedController)
{
	if (!ElimmedCharacter->IsPlayerControlled())
		return;

	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}

	if (Cast<APlayerController>(ElimmedController))
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}
