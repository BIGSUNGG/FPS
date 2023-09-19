// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include KraverGameMode_h
#include "TeamGameMode.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API ATeamGameMode : public AKraverGameMode
{
	GENERATED_BODY()
	
public:
	ATeamGameMode();
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
	virtual void DivideTeam(APlayerController* InPlayer) {}
	virtual AActor* FindRespawnPoint(AKraverPlayer* RespawnPlayer) override;

protected:
	TObjectPtr<class ATeamGameState> TeamGameState;

};
