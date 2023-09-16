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
	friend class AControlGameMode;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Getter Seeter
	virtual const TArray<APlayerState*>& GetTeamArray(ETeam FindTeam) override;

	virtual void AddTeamPlayer(ETeam InTeam, class APlayerState* InPlayer) override;

protected:
	UPROPERTY(Replicated)
	TArray<APlayerState*> BlueTeamArray;
	UPROPERTY(Replicated)
	TArray<APlayerState*> RedTeamArray;

	// Control
	UPROPERTY(Replicated)
	ETeam CurControlTeam;
	UPROPERTY(Replicated)
	float CurControlPoint = 0.f;

	// Point
	UPROPERTY(Replicated)
	int BlueTeamPoint = 0;
	UPROPERTY(Replicated)
	int RedTeamPoint = 0;

};
