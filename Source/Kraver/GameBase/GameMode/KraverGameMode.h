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

	virtual void CreatureDeath(class ACreature* DeadCreature, class AController* VictimController, AActor* AttackerActor, AController* AttackerController, FKraverDamageResult const& DamageResult);
	virtual void RequestRespawn(ACreature* ElimmedCharacter, AController* ElimmedController);

protected:

};
