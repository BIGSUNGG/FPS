// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include KraverGameState_h
#include "TeamGameState.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API ATeamGameState : public AKraverGameState
{
	GENERATED_BODY()
	
public:

public:
	// Getter Seeter
	virtual const TArray<APlayerState*>& GetTeamArray(ETeam FindTeam) { return PlayerArray; }

	virtual void AddTeamPlayer(ETeam InTeam, class APlayerState* InPlayer) {}

};
