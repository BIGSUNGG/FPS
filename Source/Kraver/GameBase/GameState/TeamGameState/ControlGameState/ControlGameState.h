// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include TeamGameState_h
#include "ControlGameState.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API AControlGameState : public ATeamGameState
{
	GENERATED_BODY()
	
public:
	// Getter Seeter
	virtual const TArray<APlayerState*>& GetTeamArray(ETeam FindTeam) override;

	virtual void AddTeamPlayer(ETeam InTeam, class APlayerState* InPlayer) override;

protected:
	TArray<APlayerState*> BlueTeamArray;
	TArray<APlayerState*> RedTeamArray;

};
