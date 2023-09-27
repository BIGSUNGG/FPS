// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include KraverPlayerState_h
#include "TeamPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API ATeamPlayerState : public AKraverPlayerState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void OnNewPawn(APawn* NewPawn) override;

public:
	// Getter Setter
	ETeam GetPlayerTeam() { return PlayerTeam; }

	void SetPlayerTeam(ETeam InTeam);

protected:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Data|Team", meta = (AllowPrivateAccess = "true"))
	ETeam PlayerTeam;

};
