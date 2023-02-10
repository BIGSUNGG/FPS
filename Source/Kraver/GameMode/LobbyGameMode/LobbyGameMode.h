// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/GameMode.h"
#include "Kraver/GameMode/KraverGameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API ALobbyGameMode : public AKraverGameMode
{
	GENERATED_BODY()

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
};
