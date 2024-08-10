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
	virtual void Logout( AController* Exiting ) override;
	// 플레이어 생성 시 호출
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;

protected:
	// 플레이어의 팀을 나눌때 호출
	virtual void DivideTeam(AController* InPlayer) {} 

protected:
	TObjectPtr<class ATeamGameState> TeamGameState;

};
