// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/GameState.h"
#include "KraverGameState.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API AKraverGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	// 플레이어 사망 시 호출
	void CreatureDeath(class ACreature* DeadCreature, class AController* VictimController, AActor* AttackerActor, AController* AttackerController, FKraverDamageResult const& DamageResult);
	// 게임 종료 시 호출
	void GameFinish(ETeam WinTeam);

protected:
	// Rpc
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_CreatureDeath(class ACreature* DeadCreature, class AController* VictimController, AActor* AttackerActor, AController* AttackerController, FKraverDamageResult const& DamageResult);
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_GameFinish(ETeam WinTeam);

public:
	FPlayerStateDeathDele OnCreatureDeath; // 플레이어 사망 시 호출
	FGameFinish OnGameFinish; // 게임 종료 시 호출

};
