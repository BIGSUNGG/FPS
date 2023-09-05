// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include KraverGameMode_h
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API ALobbyGameMode : public AKraverGameMode
{
	GENERATED_BODY()

public:
	virtual void PostLogin(APlayerController* NewPlayer) override; // 플레이어가 들어오면 호출되는 함수
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby", meta = (AllowPrivateAccess = "true"))
	int NeedPlayer = 4;

};
