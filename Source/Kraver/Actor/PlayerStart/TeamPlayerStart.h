// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kraver/Kraver.h"
#include "GameFramework/PlayerStart.h"
#include "TeamPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class KRAVER_API ATeamPlayerStart : public APlayerStart
{
	GENERATED_BODY()
	
public:
	// Getter Setter
	ETeam GetCanSpawnTeam() { return CanSpawnTeam; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data|Team", meta = (AllowPrivateAccess = "true"))
	ETeam CanSpawnTeam = ETeam::TEAM_ALL;

};
