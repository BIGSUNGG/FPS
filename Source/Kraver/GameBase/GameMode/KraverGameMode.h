// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/GameMode.h"
#include "KraverGameMode.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API AKraverGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AKraverGameMode();
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;

	virtual void CreatureDeath(class ACreature* DeadCreature, class AController* VictimController, AActor* AttackerActor, AController* AttackerController, FKraverDamageResult const& DamageResult);
	virtual void RequestRespawn(AKraverPlayer* RespawnPlayer, AController* PlayerController);
	virtual void RequsetSpectate(AController* PlayerController);

protected:
	virtual void GameFinishEvent(ETeam WinTeam);

public:
	// Getter Setter
	static float GetRespawnTime() { return RespawnTime; }

protected:
	class AKraverGameState* KraverGameState = nullptr;
	
	// Game
	bool IsGameFinish = false;

	// Respawn
	bool bRespawn = true;
	FTimerHandle RespawnTimer;
	inline static const float RespawnTime = 10.f;

	// Spectator
	bool bSpectate = true;
	FTimerHandle SpectateTimer;
	float SpectateStartTime = 2.f;
};
