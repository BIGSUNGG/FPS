// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include TeamGameMode_h
#include "ControlGameMode.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API AControlGameMode : public ATeamGameMode
{
	GENERATED_BODY()
	
public:

protected:
	virtual void DivideTeam(APlayerController* InPlayer) override;

protected:
	TObjectPtr<class AControlGameState> ControlGameState;
	//class AControlArea*

};
