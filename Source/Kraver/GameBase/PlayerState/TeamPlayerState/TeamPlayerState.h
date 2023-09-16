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
	virtual void OnPawnSetEvent(APlayerState* Player, APawn* NewPawn, APawn* OldPawn) override; // 새로운 Player 캐릭터가 생성되었을때 호출됨

public:
	void SetPlayerTeam(ETeam InTeam);

protected:
	UPROPERTY(Replicated)
	ETeam PlayerTeam;

};
