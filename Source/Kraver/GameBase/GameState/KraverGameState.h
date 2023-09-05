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
	void CreatureDeath(class ACreature* DeadCreature, class AController* VictimController, AActor* AttackerActor, AController* AttackerController, FKraverDamageResult const& DamageResult);

protected:
	// Rpc
	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_CreatureDeath(class ACreature* DeadCreature, class AController* VictimController, AActor* AttackerActor, AController* AttackerController, FKraverDamageResult const& DamageResult);

public:
	FPlayerStateDeathDele OnCreatureDeath;

};
